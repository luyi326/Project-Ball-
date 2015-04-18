#include "IRRim.h"
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include "BlobCompare.h"

#define IR_RIM_DEBUG
#define QUICK_IR_RIM_DEBUG

// Assume the starting address is 0x04 beacause @Tony broke the first two ports.
#define PV_N(n) (1 << (n+0))
#define IRRIM_SEEK_INTERVAL (600000000) //interval is in nanoseconds
#define IRRIM_RETRACK_INTERVAL (20000)
#define SERVO_STEP (20)

#define PVISION_MAX_ATTEMPT (5)

#define FULL_HORIZONTAL (1024)
#define FULL_VERTICAL (750)
#define HALF_HORIZONTAL (FULL_HORIZONTAL/2)
#define HALF_VERTICAL (FULL_VERTICAL/2)
#define LEFT_QUARTER_HORIZONTAL (FULL_HORIZONTAL/4)
#define RIGHT_QUARTER_HORIZONTAL (3*FULL_HORIZONTAL/4)

#define FULL_D_SENSOR (4.0f)
#define HALF_D_SENSOR (FULL_D_SENSOR/2)
#define TAN_16_5 (0.29621349496)
#define TAN_11_5 (0.20345229942)

#define SERVO_SEEK_TOLERANCE 0.05f
#define SERVO_FOLLOW_TOLERANCE 0.10f


float PID_kernel(PID_IRRim& pid, float error, float position);
void PID_set(PID_IRRim& PID_speed, float ig,float pg,float dg, float imax, float imin);


ostream& operator<<(ostream& os, const IR_target& t) {
	if (t.target_located) {
		os << "(" << t.angle << " degrees, " << t.distance << " centimeters)";
	} else {
		os << "(INVALID)";
	}
	return os;
}

//PID control kernel
float PID_kernel(PID_IRRim& pid, float error, float position) {
	float pTerm, dTerm, iTerm;
	pTerm = pid.pGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	pid.iState += error;
	if (pid.iState > pid.iMax) pid.iState = pid.iMax;
	else if (pid.iState < pid.iMin) pid.iState = pid.iMin;
	iTerm = pid.iGain * (pid.iState); // calculate the integral term
	dTerm = pid.dGain * (pid.dState - position);
	pid.dState = position;
	return pTerm + dTerm + iTerm;
		//return pTerm + dTerm;
}

void PID_set(PID_IRRim& PID_speed, float ig,float pg,float dg, float imax, float imin) {
	PID_speed.dGain = dg;
	PID_speed.iGain = ig;
	PID_speed.pGain = pg;
	PID_speed.iMax = imax;
	PID_speed.iMin = imin;

}

static PID_IRRim pid_front;
static PID_IRRim pid_back;

//Constructor and destructor
IRRim::IRRim(uint8_t num_of_sensors, pwmName servoPin, gpioName muxResetPin, adcName feedbackPin) :
	mux(muxResetPin),
	servo(servoPin, feedbackPin),
	sensor_count(0),
	seeking_state(IRRimState_seeking),
	current_active_pair(IRSensorPairInvalid),
	current_iteration(0),
	current_lower_bound(0),
	current_upper_bound(180),
	servo_current_position(0),
	seeking_is_upwared(true),
	dummy_target({false, 0, 0.0f}),
	last_target({false, 0, 0.0f}),
	should_reverse(false),
	o_left(-HALF_D_SENSOR, 0, 0),
	o_right(HALF_D_SENSOR, 0, 0),
	o_left_m_right(-FULL_D_SENSOR, 0, 0)
	{
	if (num_of_sensors > 6) {
		cerr << "IRRim::IRRim::Number of sensors is " << num_of_sensors << ", maximum is 6" << endl;
		exit(1);
	}

	//set PID seed
	PID_set(pid_front, 1.0f, 1.0f / 40, 1.0f / 120, 3, 0);
	PID_set(pid_back, 1.0f, 1.0f / 40, 1.0f / 120, 3, 0);

	//Initialize all IRs and check status
	sensors = new PVision[num_of_sensors];
	sensor_count = num_of_sensors;
	bool PVision_inited = false;
	uint8_t PVision_init_attempt = 0;
	while (PVision_init_attempt < PVISION_MAX_ATTEMPT && !PVision_inited) {
		mux.reset();
		for (int i = 0; i < sensor_count; i++) {
			#ifdef IR_RIM_DEBUG
			cout << "IRRim::IRRim::Initializing sensor No. " << i + 1 << endl;
			#endif
			mux.selectChannel(PV_N(i));
			if (!sensors[i].init()) {
				cerr << "IRRim::IRRim::Sensor No. " << i + 1 << " not initialized correctly" << endl;
				continue;
			}
			#ifdef IR_RIM_DEBUG
			cout << "IRRim::IRRim::Sensor No. " << i + 1 << " done." << endl;
			#endif
		}
		PVision_inited = true;
	}
	if (!PVision_inited) {
		cerr << "IRRim::IRRim::Attempted " << PVISION_MAX_ATTEMPT << " times to initialize PVision and failed" << endl;
		throw naughty_exception_PVisionInitFail;
	}
	servo.calibrate();
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::IRRim::Servo calibration complete" << endl;
	#endif
	servo.set_tolerance(SERVO_SEEK_TOLERANCE);
	servo.move_to(0);

	usleep(100); // Allow initial setup time

}

IRRim::~IRRim() {
	mux.reset();
	// delete sensors;
}

/**
 * [IRRim::reset description]
 */
void IRRim::reset() {
	mux.reset();
}

IR_target IRRim::run() {
	switch (seeking_state) {
		case IRRimState_seeking:
			seek();
			return dummy_target;
		case IRRimState_targetFound:
			return follow(current_active_pair);
		case IRRimState_reversing:
			reverse();
			return last_target;
	}
	return dummy_target;
}


void IRRim::force_seek() {
	if (seeking_state == IRRimState_seeking) return;
	seeking_state = IRRimState_seeking;
	servo_current_position = 0;
	current_active_pair = IRSensorPairInvalid;
	servo.set_tolerance(SERVO_SEEK_TOLERANCE);
	// current_lower_bound = current_upper_bound = servo_current_position;
}

void IRRim::seek() {
	// static timespec last_arrive_time;
	servo.move_to(servo_current_position);
	if (servo.target_position_reached()) {
		if (abs(servo_current_position - static_cast<float>(current_lower_bound)) < 0.00001) {
			servo_current_position = current_upper_bound;
			if (current_lower_bound > 0) current_lower_bound -= 10;
			if (current_lower_bound < 0) current_lower_bound = 0;
			#ifdef IR_RIM_DEBUG
			cout << "IRRim::seek::Moving to " << int(current_upper_bound) << endl;
			#endif
			// clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &last_arrive_time);
		} else {
			servo_current_position = current_lower_bound;
			if (current_upper_bound < 180) current_upper_bound += 10;
			if (current_upper_bound > 180) current_upper_bound = 180;
			#ifdef IR_RIM_DEBUG
			cout << "IRRim::seek::Moving to " << int(current_lower_bound) << endl;
			#endif
			// clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &last_arrive_time);
		}
	}
	inspect_sensors();
	servo.move_to(servo_current_position);
}

// static int lostTargetCount = 0;
IR_target IRRim::follow(IRSensorPair following_pair) {
	if (following_pair == IRSensorPairInvalid) {
		cerr << "IRRim::follow::Active sensor pair not selected, fatal error" << endl;
		exit(1);
	}
	//experimental code for PID control start
	//step1 get camera position
	Blob left_avg, right_avg;
	IRReadResult ir_state;
	try {
		ir_state = read_IR(following_pair, &left_avg, &right_avg);
	} catch (const std::ios_base::failure& e) {
        cerr << "Under flow happened in read_IR" << endl;
        throw e;
    }
	// cout << "left coordinate: " << left_avg << ", right coordinate: " << right_avg << endl;
	//step2 calculate camera values
	int middle_point = 0;
	int middle_err = 0;
	IR_target new_target;
	new_target.target_located = true;
	switch (ir_state) {
		case IRReadResultLost:
			timespec tmp;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);
			timespec diff;
			if ((tmp.tv_nsec - target_last_seen.tv_nsec) < 0) {
				diff.tv_sec = tmp.tv_sec - target_last_seen.tv_sec - 1;
				diff.tv_nsec = 1000000000 + tmp.tv_nsec - target_last_seen.tv_nsec;
			} else {
				diff.tv_sec = tmp.tv_sec - target_last_seen.tv_sec;
				diff.tv_nsec = tmp.tv_nsec - target_last_seen.tv_nsec;
			}
			// timespec diff time_diff(tmp, target_last_seen);
			if (diff.tv_sec != 0 || diff.tv_nsec >= 50000000) {
				#ifdef QUICK_IR_RIM_DEBUG
				cout << "IRRim::follow::Target lost, going back to seeking" << endl;
				#endif
				seeking_state = IRRimState_seeking;
				current_active_pair = IRSensorPairInvalid;
				servo.set_tolerance(SERVO_SEEK_TOLERANCE);
				servo_current_position = 0;
				// current_lower_bound = current_upper_bound = servo_current_position;
				return dummy_target;
			}
			return last_target;
		case IRReadResultBlobOnLeft:
			middle_point = -left_avg.X;
			#ifdef QUICK_IR_RIM_DEBUG
			// cout << "IR is on left" << endl;
			cout << "IRRim::follow::Left middle_point: " << middle_point;
			cout << "IRRim::follow::left coordinate: " << left_avg << ", right coordinate: " << right_avg << endl;
			#endif
			// cout << "Servo moving clockwise" << endl;
			// exit(0);
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen);
			new_target.angle = servo.current_position();
			new_target.distance = -1.0f;
			break;
		case IRReadResultBlobOnRight:
			middle_point = FULL_HORIZONTAL - right_avg.X;
			#ifdef QUICK_IR_RIM_DEBUG
			// cout << "IR is on right" << endl;
			cout << "IRRim::follow::Right middle_point: " << middle_point;
			cout << "IRRim::follow::left coordinate: " << left_avg << ", right coordinate: " << right_avg << endl;;
			#endif
			// cout << "Servo moving counterclockwise" << endl;
			// exit(0);
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen);
			new_target.angle = servo.current_position();
			new_target.distance = -1.0f;
			break;
		case IRReadResultMiddle:
			vec target_location = calculate_target_coordinate(left_avg.X, left_avg.Y, right_avg.X, right_avg.Y);
			middle_point = FULL_HORIZONTAL - right_avg.X - left_avg.X;
			#ifdef QUICK_IR_RIM_DEBUG
			// cout << "IR is in the middle" << endl;
			// cout << "Mid point: " << calculate_target_coordinate(sensors[0].Blob1.X, sensors[0].Blob1.Y, sensors[1].Blob1.X, sensors[1].Blob1.Y) << endl;
			// cout << "Target location: " << target_location << endl;
			cout << "IRRim::follow::middle_point: " << middle_point;
			cout << "IRRim::follow::left coordinate: " << left_avg << ", right coordinate: " << right_avg << endl;
			#endif
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen);
			new_target.angle = servo.current_position();
			new_target.distance = - target_location.z;
		break;
	}

	if (following_pair == IRSensorPairBack) {
		new_target.angle += 180;
	}

	last_target = new_target;

	if (middle_point > 100) {
		middle_err = middle_point - 100;
	} else if (middle_point < -100) {
		middle_err = middle_point + 100;
	} else {
		middle_err = 0;
		return new_target;
	}
	//step3 put value and desire posi tion into PID
	int correct = 0;
	if (following_pair == IRSensorPairFront) {
		correct = int(lround(PID_kernel(pid_front, middle_err, middle_point)));
	} else if (following_pair == IRSensorPairBack) {
		correct = int(lround(PID_kernel(pid_back, middle_err, middle_point)));
	}
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::follow::Correction is: " << correct << endl;
	#endif
	if (correct > 1) {
		correct = correct - 1;
	} else if (correct < -1) {
		correct = correct + 1;
	} else {
		correct = 0;
	}
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::follow::Correction is now after correction trimming: " << correct << endl;
	#endif
	servo_current_position = servo.current_position() - correct;
	// servo_current_position = (int)servo.current_position();
	//add error output to servo
	if (servo_current_position < 0) {
		servo_current_position = 0;
		seeking_state = IRRimState_reversing;
		should_reverse = true;
	}
	if (servo_current_position > 180) {
		servo_current_position = 180;
		seeking_state = IRRimState_reversing;
		should_reverse = true;
	}
	servo.move_to(servo_current_position);

	return new_target;
}

void IRRim::reverse() {
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::reverse::Reversing" << endl;
	#endif
	//if the current position is floating 0
	if (should_reverse) {
		if (abs(servo_current_position) < 0.00001) {
			servo_current_position = 180;
		} else {
			servo_current_position = 0;
		}
		should_reverse = false;
	} else {
		if (servo.target_position_reached()) {
			seeking_state = IRRimState_seeking;
		}
	}
	inspect_sensors();
	servo.move_to(servo_current_position);
}

void IRRim::inspect_sensors() {
	if (read_IR(IRSensorPairFront) != IRReadResultLost) {
		servo_current_position = (int)servo.current_position();
		#ifdef IR_RIM_DEBUG
		cout << "IRRim::seek::Saw a target on front, entering follow mode" << endl;
		cout << "IRRim::seek::Servo redirecting to " << int(servo_current_position) << endl;
		#endif
		seeking_state = IRRimState_targetFound;
		current_active_pair = IRSensorPairFront;
		servo.set_tolerance(SERVO_FOLLOW_TOLERANCE);
	} else if (read_IR(IRSensorPairBack) != IRReadResultLost) {
		servo_current_position = (int)servo.current_position();
		#ifdef IR_RIM_DEBUG
		cout << "IRRim::seek::Saw a target on back, entering follow mode" << endl;
		cout << "IRRim::seek::Servo redirecting to " << int(servo_current_position) << endl;
		#endif
		seeking_state = IRRimState_targetFound;
		current_active_pair = IRSensorPairBack;
		servo.set_tolerance(SERVO_FOLLOW_TOLERANCE);
	}
}

IRReadResult IRRim::read_IR(IRSensorPair pair, Blob* _left_avg, Blob* _right_avg) {
	uint8_t result1, result2;
	PVision* pv1;
	PVision* pv2;
	switch (pair) {
		case IRSensorPairFront:
			mux.selectChannel(PV_N(1));
			try {
				result1 = sensors[1].readBlob();
			} catch (naughty_exception e) {
				switch (e) {
					case naughty_exception_PVisionReadFail:
					case naughty_exception_PVisionWriteFail:
					case naughty_exception_MuxWriteFail:
					case naughty_exception_I2CError:
						cerr << "IRRim::read_IR:: sensor No. 1 error" << endl;
					default:
					break;
					// throw e;
				}
			} catch (const std::ios_base::failure& e) {
	            cerr << "Under flow happened in sensor 1" << endl;
	            throw e;
	        }
			mux.selectChannel(PV_N(0));
			try {
				result2 = sensors[0].readBlob();
			} catch (naughty_exception e) {
				switch (e) {
					case naughty_exception_PVisionReadFail:
					case naughty_exception_PVisionWriteFail:
					case naughty_exception_MuxWriteFail:
					case naughty_exception_I2CError:
						cerr << "IRRim::read_IR:: sensor No. 0 error" << endl;
					default:
					break;
					// throw e;
				}
			} catch (const std::ios_base::failure& e) {
	            cerr << "Under flow happened in sensor 0" << endl;
	            throw e;
	        }
			pv1 = &(sensors[1]);
			pv2 = &(sensors[0]);
		break;
		case IRSensorPairBack:
			mux.selectChannel(PV_N(2));
			try {
				result1 = sensors[2].readBlob();
			} catch (naughty_exception e) {
				switch (e) {
					case naughty_exception_PVisionReadFail:
					case naughty_exception_PVisionWriteFail:
					case naughty_exception_MuxWriteFail:
					case naughty_exception_I2CError:
						cerr << "IRRim::read_IR:: sensor No. 2 error" << endl;
					default:
					break;
					// throw e;
				}
			} catch (const std::ios_base::failure& e) {
	            cerr << "Under flow happened in sensor 2" << endl;
	            throw e;
	        }
			mux.selectChannel(PV_N(3));
			try {
				result2 = sensors[3].readBlob();
			} catch (naughty_exception e) {
				switch (e) {
					case naughty_exception_PVisionReadFail:
					case naughty_exception_PVisionWriteFail:
					case naughty_exception_MuxWriteFail:
					case naughty_exception_I2CError:
						cerr << "IRRim::read_IR:: sensor No. 3 error" << endl;
					default:
					break;
					// throw e;
				}
			} catch (const std::ios_base::failure& e) {
	            cerr << "Under flow happened in sensor 3" << endl;
	            throw e;
	        }
			pv1 = &(sensors[2]);
			pv2 = &(sensors[3]);
		break;
		case IRSensorPairInvalid:
			cerr << "IRRim::read_IR::Trying to read IR data from invalid sensor pair" << endl;
			return IRReadResultLost;
	}

	// BlobCluster* normalized_result = normalize(result1, result2, pv1, pv2);
	Blob left_avg = average(result1, pv1);
	Blob right_avg = average(result2, pv2);
	if (_left_avg != NULL) {
		_left_avg->X = left_avg.X;
		_left_avg->Y = left_avg.Y;
		_left_avg->Size = left_avg.Size;
	}
	if (_right_avg != NULL) {
		_right_avg->X = right_avg.X;
		_right_avg->Y = right_avg.Y;
		_right_avg->Size = right_avg.Size;
	}
	if (Blob_is_valid(left_avg) || Blob_is_valid(right_avg)) {
		if (Blob_is_valid(left_avg) && Blob_is_valid(right_avg)) {
			//When Blob1 is in vision of both left and right cameras, if the Blobs are on edge of the cameras, location of the
			//blob will still be recognized as not in the middle (left or right).
			// if (left_avg.X < LEFT_QUARTER_HORIZONTAL && right_avg.X < LEFT_QUARTER_HORIZONTAL) {
			// 	return IRReadResultBlobOnLeft;
			// }
			// if (left_avg.X > RIGHT_QUARTER_HORIZONTAL && right_avg.X > RIGHT_QUARTER_HORIZONTAL) {
			// 	return IRReadResultBlobOnRight;
			// }
			return IRReadResultMiddle;
		} else if (Blob_is_valid(left_avg)) {
			return IRReadResultBlobOnLeft;
		} else if (Blob_is_valid(right_avg)) {
			return IRReadResultBlobOnRight;
		}
	}
	// cout << "left can see " << normalized_result[0].validBlobCount << " blobs and right can see " << normalized_result[1].validBlobCount << endl;
	return IRReadResultLost;

}

IRReadResult IRRim::read_IR(IRSensorPair pair) {
	return read_IR(pair, NULL, NULL);
}

void IRRim::nextSensor() {
}

void IRRim::select(uint8_t num) {
}


inline void validateBlob(uint8_t index_left, uint8_t index_right) {

}

//Z positioning algos starts here

inline vec IRRim::calculate_target_coordinate(int left_x, int left_y, int right_x, int right_y) {
	vec dir_left = get_directional_vec(left_x, left_y);
	vec dir_right = get_directional_vec(right_x, right_y);
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::calculate_target_coordinate::directional left: " << dir_left << endl;
	cout << "IRRim::calculate_target_coordinate::directional right: " << dir_right << endl;
	#endif
	float z_left, z_right;
	calculate_intersection_point(dir_left, dir_right, z_left, z_right);
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::calculate_target_coordinate::z_left = " << z_left << " z_right = " << z_right << endl;
	#endif
	vec s_C = o_left + (dir_left * z_left);
	vec t_C = o_right + (dir_right * z_right);
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::calculate_target_coordinate::s_C = " << s_C << " t_C = " << t_C << endl;
	#endif
	vec mid = vec::mid_point(s_C, t_C);
	#ifdef IR_RIM_DEBUG
	cout << "IRRim::calculate_target_coordinate::mid = " << mid << endl;
	#endif
	return mid;
}

inline vec IRRim::get_directional_vec(int x, int y) {
	vec result;
	result.x = ((float)(x - HALF_HORIZONTAL)) * TAN_16_5 / HALF_HORIZONTAL;
	result.y = ((float)(y - HALF_VERTICAL)) * TAN_11_5 / HALF_VERTICAL;
	result.z = 1.0f;
	return result;
}

inline void IRRim::calculate_intersection_point(vec directional_left, vec directional_right, float& z_left, float& z_right) {
	float a = vec::dot_product(directional_left, directional_left);
	float b = vec::dot_product(directional_left, directional_right);
	float c = vec::dot_product(directional_right, directional_right);
	float d = vec::dot_product(directional_left, o_left_m_right);
	float e = vec::dot_product(directional_right, o_left_m_right);

	float ac_m_b_sq = a * c - b * b;
	if (ac_m_b_sq == 0) {
		cerr << "IRRim::calculate_target_coordinate::The two directional vectors are parallel!!! Please check implementation or sensor placement" << endl;
		z_left = 0.0f;
		z_right = 0.0f;
	}
	z_left = (b * e - c * d) / ac_m_b_sq;
	z_right = (a * e - b * d) / ac_m_b_sq;
}

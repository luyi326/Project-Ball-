#include "IRRim.h"
#include <stdlib.h>
#include <unistd.h>
#include "BlobCompare.h"

#define IR_RIM_DEBUG

// Assume the starting address is 0x04 beacause @Tony broke the first two ports.
#define PV_N(n) (1 << (n+2))
#define IRRIM_SEEK_INTERVAL (600000000) //interval is in nanoseconds
#define IRRIM_RETRACK_INTERVAL (20000)
#define SERVO_STEP (20)

#define FULL_HORIZONTAL (1000)
#define FULL_VERTICAL (750)
#define HALF_HORIZONTAL (FULL_HORIZONTAL/2)
#define HALF_VERTICAL (FULL_VERTICAL/2)

#define FULL_D_SENSOR (4.0f)
#define HALF_D_SENSOR (FULL_D_SENSOR/2)
#define TAN_16_5 (0.29621349496)
#define TAN_11_5 (0.20345229942)

//Constructor and destructor
IRRim::IRRim(uint8_t num_of_sensors, pwmName servoPin, gpioName muxResetPin, adcName feedbackPin) :
	mux(muxResetPin),
	servo(servoPin, feedbackPin),
	sensor_count(0),
	state(IRRimState_seeking),
	current_iteration(0),
	current_lower_bound(0),
	current_upper_bound(180),
	servo_current_position(0),
	is_seeking(true),
	seeking_is_upwared(true),
	o_left(-HALF_D_SENSOR, 0, 0),
	o_right(HALF_D_SENSOR, 0, 0),
	o_left_m_right(-FULL_D_SENSOR, 0, 0)
	{
	if (num_of_sensors > 6) {
		cerr << "Number of sensors is " << num_of_sensors << ", maximum is 6" << endl;
		exit(1);
	}


	//Initialize all IRs and check status
	sensors = new PVision[num_of_sensors];
	sensor_count = num_of_sensors;
	for (int i = 0; i < sensor_count; i++) {
		cout << "Initializing sensor No. " << i + 1 << endl;
		mux.selectChannel(PV_N(i));
		if (!sensors[i].init()) {
			cerr << "Sensor No. " << i + 1 << " not initialized correctly" << endl;
		} else {
			//TODO: Fill this place with apporiate logger: spdLogger, Boost logger etc..
		}
		cout << "Sensor No. " << i + 1 << " done." << endl;
	}
	servo.calibrate();
	cout << "Servo calibration complete" << endl;
	servo.move_to(0);

	usleep(100); // Allow initial setup time

	//All ready, record clock and start
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
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

void IRRim::run() {
	if (is_seeking) {
		seek();
	} else {
		follow();
	}
}

void IRRim::seek() {
	if (servo.target_position_reached()) {
		if (servo_current_position == 0) {
			servo_current_position = 180;
		} else {
			servo_current_position = 0;
		}
	}
	if (read_IR(IRSensorPairFront)) {
		servo_current_position = (int)servo.current_position();
		is_seeking = false;
	}
	servo.move_to(servo_current_position);
}

// static int lostTargetCount = 0;
void IRRim::follow() {
	servo.move_to(servo_current_position);
	// timespec tmp;
	// clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);

	// timespec diff = time_diff(current_time, tmp);
	// 	if (diff.tv_sec == 0 && diff.tv_nsec >= IRRIM_SEEK_INTERVAL) {
	// 	if (read_IR(0)) {
	// 		lostTargetCount = 0;
	// 		if (sensors[0].Blob1.X < 300) {
	// 			if (servo_current_position < 0) {
	// 				cout << "servo out of range. " << endl;
	// 			}
	// 			servo.move_to(--servo_current_position);
	// 		} else if (sensors[0].Blob1.X > 700) {
	// 			servo.move_to(++servo_current_position);
	// 			if (servo_current_position > 180) {
	// 				cout << "servo out of range. " << endl;
	// 			}
	// 		}
	// 		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
	// 	} else {
	// 		lostTargetCount++;
	// 		if (lostTargetCount > 4) {
	// 			cout << "Lost target!!" << endl;
	// 			current_lower_bound = servo_current_position - 10;
	// 			current_upper_bound = servo_current_position + 10;
	// 			current_iteration = 0;
	// 			if (current_lower_bound < 0) current_lower_bound = 0;
	// 			if (current_upper_bound > 180) current_upper_bound = 180;
	// 			is_seeking = true;
	// 		} else {
	// 			cout << "Lost target " << lostTargetCount << " times" << endl;
	// 		}
	// 	}
	// } else {

	// }

}

bool IRRim::read_IR(IRSensorPair pair) {
	uint8_t result1, result2;
	PVision* pv1;
	PVision* pv2;
	switch (pair) {
		case IRSensorPairFront:
			mux.selectChannel(PV_N(0));
			result1 = sensors[0].readBlob();
			mux.selectChannel(PV_N(1));
			result2 = sensors[1].readBlob();
			pv1 = &(sensors[0]);
			pv2 = &(sensors[1]);
		break;
		case IRSensorPairBack:
			mux.selectChannel(PV_N(2));
			result1 = sensors[2].readBlob();
			mux.selectChannel(PV_N(3));
			result2 = sensors[3].readBlob();
			pv1 = &(sensors[2]);
			pv2 = &(sensors[3]);
		break;
	}

	BlobCluster* normalized_result = normalize(result1, result2, pv1, pv2);
	if (normalized_result[0].validBlobCount != 0 || normalized_result[0].validBlobCount != 1) {
		return true;
	}

	// if ((result1 & BLOB1) && (result2 & BLOB1))
	// {
	// 	is_seeking = false;
	// 	#ifdef IR_RIM_DEBUG
	// 	cout << 0 <<  " BLOB1 detected. X:" << sensors[0].Blob1.X << " Y:" << sensors[0].Blob1.Y;
	// 	cout << " Size: " << sensors[0].Blob1.Size << endl;

	// 	cout << 1 <<  " BLOB1 detected. X:" << sensors[1].Blob1.X << " Y:" << sensors[1].Blob1.Y;
	// 	cout << " Size: " << sensors[1].Blob1.Size << endl;
	// 	#endif
	// 	calculate_target_coordinate(sensors[0].Blob1.X, sensors[0].Blob1.Y, sensors[1].Blob1.X, sensors[1].Blob1.Y);

	// 	return true;
	// } else {
	// 	// is_seeking = true;
	// }
	return false;
}

void IRRim::nextSensor() {
}

void IRRim::select(uint8_t num) {
}


inline void validateBlob(uint8_t index_left, uint8_t index_right) {

}

inline timespec IRRim::time_diff(timespec t1, timespec t2) {
	timespec temp;
	if ((t2.tv_nsec - t1.tv_nsec) < 0) {
		temp.tv_sec = t2.tv_sec - t1.tv_sec - 1;
		temp.tv_nsec = 1000000000 + t2.tv_nsec - t1.tv_nsec;
	} else {
		temp.tv_sec = t2.tv_sec - t1.tv_sec;
		temp.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	}
	return temp;
}

//Z positioning algos starts here

inline void IRRim::calculate_target_coordinate(int left_x, int left_y, int right_x, int right_y) {
	vec dir_left = get_directional_vec(left_x, left_y);
	vec dir_right = get_directional_vec(right_x, right_y);
	#ifdef IR_RIM_DEBUG
	cout << "directional left: " << dir_left << endl;
	cout << "directional right: " << dir_right << endl;
	#endif
	float z_left, z_right;
	calculate_intersection_point(dir_left, dir_right, z_left, z_right);
	#ifdef IR_RIM_DEBUG
	cout << "z_left = " << z_left << " z_right = " << z_right << endl;
	#endif
	vec s_C = o_left + (dir_left * z_left);
	vec t_C = o_right + (dir_right * z_right);
	#ifdef IR_RIM_DEBUG
	cout << "s_C = " << s_C << " t_C = " << t_C << endl;
	#endif
	vec mid = vec::mid_point(s_C, t_C);
	#ifdef IR_RIM_DEBUG
	cout << "mid = " << mid << endl;
	#endif
}

inline vec IRRim::get_directional_vec(int x, int y) {
	vec result;
	result.x =((float)(x - HALF_HORIZONTAL)) * TAN_16_5 / HALF_HORIZONTAL;
	result.y =((float)(y - HALF_VERTICAL)) * TAN_11_5 / HALF_VERTICAL;
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
		cerr << "The two directional vectors are parallel!!! Please check implementation or sensor placement" << endl;
		z_left = 0.0f;
		z_right = 0.0f;
	}
	z_left = (b * e - c * d) / ac_m_b_sq;
	z_right = (a * e - b * d) / ac_m_b_sq;
}

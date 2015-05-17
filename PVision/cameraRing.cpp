#include "cameraRing.h"
#include "BlobCompare.h"

#define CAMERA_RIGN_DEBUG

#define FRONT_LEFT (1 << 0)
#define FRONT_RIGHT (1 << 1)
#define BACK_LEFT (1 << 2)
#define BACK_RIGHT (1 << 3)

#define FULL_D_SENSOR (4.0f)
#define HALF_D_SENSOR (FULL_D_SENSOR/2)
#define PI (3.14159265)
#define HALF_PI (1.57079632)
#define TAN_16_5 (0.29621349496)
#define TAN_11_5 (0.20345229942)

#define SERVO_SEEK_TOLERANCE (0.05f)
#define SERVO_FOLLOW_TOLERANCE (0.10f)

#define SPHERE_DEGREE_ZERO (0)
#define SPHERE_DEGREE_QUARTER (90)
#define SPHERE_DEGREE_HALF (180)
#define SPHERE_DEGREE_TQUARTER (270)
#define SPHERE_DEGREE_FULL (360)

// #define SERVO_ANGLE_BIAS (-88)
#define SERVO_ANGLE_BIAS (-90)


#define FULL_HORIZONTAL (1000)
#define FULL_VERTICAL (750)
#define HALF_HORIZONTAL (FULL_HORIZONTAL/2)
#define HALF_VERTICAL (FULL_VERTICAL/2)
#define LEFT_QUARTER_HORIZONTAL (FULL_HORIZONTAL/4)
#define RIGHT_QUARTER_HORIZONTAL (3*FULL_HORIZONTAL/4)

#define FULL_D_SENSOR (4.0f)
#define HALF_D_SENSOR (FULL_D_SENSOR/2)
#define PI (3.14159265)
#define HALF_PI (1.57079632)
#define TAN_16_5 (0.29621349496)
#define TAN_11_5 (0.20345229942)

#include "cameraRingPID.h"

static PID_cameraRing pid_front;
static PID_cameraRing pid_back;

cameraRing::cameraRing(pwmName servoPin, gpioName muxResetPin, adcName feedbackPin) :
	mux(muxResetPin),
	frontLeft(),
	frontRight(),
	backLeft(),
	backRight(),
	servo(servoPin, feedbackPin),
	front_angle_low(0),
	front_angle_high(0),
	back_angle_low(0),
	back_angle_high(0),
	following_pair(cameraRingSensorPairInvalid),
	servo_current_edge_is_first(true),
	servo_current_target(0),
	lastSeenTarget{NAN, NAN, NAN, false, false, false},
	searchState(cameraRingState_stopped),
	seekingState(cameraRingSeeking_notSeeking),
	operationQueue()
{
	//Initialize PVisions
	mux.selectChannel(FRONT_LEFT);
	frontLeft.init();
	mux.selectChannel(FRONT_RIGHT);
	frontRight.init();
	mux.selectChannel(BACK_LEFT);
	backLeft.init();
	mux.selectChannel(BACK_RIGHT);
	backRight.init();

	//Calibrate servo motor
	servo.calibrate();
	#ifdef CAMERA_RIGN_DEBUG
	cout << "IRRim::IRRim::Servo calibration complete" << endl;
	#endif
	servo.set_tolerance(SERVO_SEEK_TOLERANCE);
	servo.move_to(SPHERE_DEGREE_ZERO);
}

void cameraRing::scanRange(int angle_low, int angle_high) {
	if (targetLocked()) {
		//If target is locked, ignore scan request from proximity ring
		return;
	}
	//Remove extra operations that hasn't been executing yet.
	shortenQueue();

	//First map the range to the range of the camera
	angle_low = 360 - publicAngleToServoAngle(angle_low);
	angle_high = 360 -publicAngleToServoAngle(angle_high);
	int tmp = angle_low;
	angle_low = angle_high;
	angle_high = tmp;

	// cout << "In servo coordinate, low = " << angle_low << " high = " << angle_high << endl;

	if (angle_low <= angle_high) {
		if (angle_low >= SPHERE_DEGREE_ZERO && angle_high <= SPHERE_DEGREE_HALF) {
			//Only use back camera
			seekingState = cameraRingSeeking_front;
			front_angle_low = angle_low;
			front_angle_high = angle_high;

		} else if (angle_low >= SPHERE_DEGREE_HALF && angle_high <= SPHERE_DEGREE_FULL) {
			//Only use front camera
			seekingState = cameraRingSeeking_back;
			back_angle_low = angle_low - 180;
			back_angle_high = angle_high - 180;
		} else if (angle_low <= SPHERE_DEGREE_HALF && angle_high >= SPHERE_DEGREE_HALF) {
			//back camera take left half and front camera take right half
			seekingState = cameraRingSeeking_frontBack;
			front_angle_low = angle_low;
			front_angle_high = 180;
			back_angle_low = 0;
			back_angle_high = angle_high - 180;
		} else {
			//something is wrong, the target is present on more than half the sphere
			cerr << "cameraRing::scanRange::Angle Low is " << angle_low << " angle high is " << angle_high << ", this is impossible" << endl;
			exit(1);
		}
	} else {
		if (angle_low >= SPHERE_DEGREE_HALF && angle_high >= SPHERE_DEGREE_ZERO) {
			seekingState = cameraRingSeeking_backFront;
			back_angle_low = angle_low - 180;
			back_angle_high = 180;
			front_angle_low = 0;
			front_angle_high = angle_high;

		} else {
			//something is wrong, the target is present on more than half the sphere
			cerr << "cameraRing::scanRange::Angle Low is " << angle_low << " angle high is " << angle_high << ", this is impossible" << endl;
			exit(1);
		}
	}
	// cout << "fl: " << front_angle_low << " fh: " << front_angle_high << " bl: " << back_angle_low << " bh: " << back_angle_high << endl;
	//Start the module in seeking mode
	searchState = cameraRingState_seeking;
	encodeOprations();

	run();
}

void cameraRing::run() {
	switch (searchState) {
		case cameraRingState_seeking:
			seek();
			break;
		case cameraRingState_targetFound:
			follow();
			break;
		case cameraRingState_reversing:
			break;
		default:
		case cameraRingState_stopped:
		//Doing nothing
			break;
	}
}

void cameraRing::seek() {
	static bool firstTime = true;
	static timespec lastUpdateTime;
	if (firstTime) {
		firstTime = false;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &lastUpdateTime);
	}

	timespec tmp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);
	timespec diff;
	if ((tmp.tv_nsec - lastUpdateTime.tv_nsec) < 0) {
		diff.tv_sec = tmp.tv_sec - lastUpdateTime.tv_sec - 1;
		diff.tv_nsec = 1000000000 + tmp.tv_nsec - lastUpdateTime.tv_nsec;
	} else {
		diff.tv_sec = tmp.tv_sec - lastUpdateTime.tv_sec;
		diff.tv_nsec = tmp.tv_nsec - lastUpdateTime.tv_nsec;
	}

	if (servo.target_position_reached() || (diff.tv_sec != 0 || diff.tv_nsec >= 500000000)) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &lastUpdateTime);

		cameraRingSensorPair currentPair = cameraRingSensorPairInvalid;

		if (servo_current_edge_is_first) {
			servo_current_edge_is_first = false;
			cameraRingOperation currentOp = operationQueue.front();
			servo_current_target = currentOp.high;
			currentPair = currentOp.pair;
		} else {
			operationQueue.pop();
			servo_current_edge_is_first = true;
			if (operationQueue.empty()) {
				//Generate repetitive actions
				encodeOprations();
			}
			cameraRingOperation currentOp = operationQueue.front();
			servo_current_target = currentOp.low;
			currentPair = currentOp.pair;
			// cout << "Moving servo to " << servo_current_target << " with " << currentOp.pair << endl;

		}
		if (readCamera(currentPair) != cameraRingReadResultLost) {
			searchState = cameraRingState_targetFound;
			servo.set_tolerance(SERVO_FOLLOW_TOLERANCE);
		}
	}
	servo.move_to(servo_current_target);
}

cameraRingReadResult cameraRing::readCamera(cameraRingSensorPair pair, Blob* o_left_avg, Blob* o_right_avg) {
	uint8_t leftResult = 0x00;
	uint8_t rightResult = 0x00;
	Blob avgLeft;
	Blob avgRight;
	switch (pair) {
		case cameraRingSensorPairFront: {
			// cout << "Reading front" << endl;
			leftResult = retrieveCameraResult(cameraRingCameraFrontLeft);
			rightResult = retrieveCameraResult(cameraRingCameraFrontRight);
			// cout << int(leftResult) << " " << int(rightResult) << endl;
			avgLeft = average(leftResult, &frontLeft);
			avgRight = average(rightResult, &frontRight);
			cout << avgLeft << " " << avgRight << endl;
		}
		case cameraRingSensorPairBack: {
			leftResult = retrieveCameraResult(cameraRingCameraBackLeft);
			rightResult = retrieveCameraResult(cameraRingCameraBackRight);
			// cout << int(leftResult) << " " << int(rightResult) << endl;
			avgLeft = average(leftResult, &backLeft);
			avgRight = average(rightResult, &backRight);
			break;
		}
		default:
		case cameraRingSensorPairInvalid:
			cerr << "cameraRing::readCamera::Invalid sensor pairs received: " << pair << endl;
			return cameraRingReadResultLost;
	}
	if (o_left_avg != NULL) {
		o_left_avg->X = avgLeft.X;
		o_left_avg->Y = avgLeft.Y;
		o_left_avg->Size = avgLeft.Size;
	}
	if (o_right_avg != NULL) {
		o_right_avg->X = avgRight.X;
		o_right_avg->Y = avgRight.Y;
		o_right_avg->Size = avgRight.Size;
	}

	if (Blob_is_valid(avgLeft) || Blob_is_valid(avgRight)) {
		cout << "Valid" << endl;
		if (Blob_is_valid(avgLeft) && Blob_is_valid(avgRight)) {
			cout << avgLeft << endl;
			cout << avgRight << endl;
			return cameraRingReadResultMiddle;
		} else if (Blob_is_valid(avgLeft)) {
			cout << avgLeft << endl;
			return cameraRingReadResultBlobOnLeft;
		} else if (Blob_is_valid(avgRight)) {
			cout << avgRight << endl;
			return cameraRingReadResultBlobOnRight;
		}
	} else {
		// cout << avgLeft << " " << avgRight << endl;

	}
	return cameraRingReadResultLost;
}

uint8_t cameraRing::retrieveCameraResult(cameraRingCamera camIndex) {
	uint8_t result;
	try {
		switch (camIndex) {
			case cameraRingCameraFrontLeft:
				mux.selectChannel(FRONT_LEFT);
				result = frontLeft.readBlob();
				// cout << int(result) << endl;
				break;
			case cameraRingCameraFrontRight:
				mux.selectChannel(FRONT_RIGHT);
				result = frontRight.readBlob();
				// cout << int(result) << endl;
				break;
			case cameraRingCameraBackLeft:
				mux.selectChannel(BACK_LEFT);
				result = backLeft.readBlob();
				// cout << int(result) << endl;
				break;
			case cameraRingCameraBackRight:
				mux.selectChannel(BACK_RIGHT);
				result = backRight.readBlob();
				// cout << int(result) << endl;
				break;
			default:
				cerr << "cameraRing::retrieveCameraResult::Invalid camera index" << int(camIndex) << endl;
				break;
		}
	} catch (naughty_exception e) {
		switch (e) {
			case naughty_exception_PVisionReadFail:
			case naughty_exception_PVisionWriteFail:
			case naughty_exception_MuxWriteFail:
			case naughty_exception_I2CError:
				cerr << "cameraRing::retrieveCameraResult:: sensor No. " << int(camIndex) << " error" << endl;
				result = 0x00;
			default:
			break;
			// throw e;
		}
	} catch (const std::ios_base::failure& e) {
		cerr << "cameraRing::retrieveCameraResultUnder flow happened in sensor " << int(camIndex) << endl;
		throw e;
	}
	return result;
}

camearaRingTargetInfo cameraRing::follow() {
	if (following_pair == cameraRingSensorPairInvalid) {
		cerr << "cameraRing::follow::Active sensor pair not selected, fatal error" << endl;
		exit(1);
	}
	//experimental code for PID control start
	//step1 get camera position
	Blob left_avg, right_avg;
	cameraRingReadResult ir_state;
	try {
		ir_state = readCamera(following_pair, &left_avg, &right_avg);
	} catch (const std::ios_base::failure& e) {
        cerr << "cameraRing::follow::Underflow happened in read_IR" << endl;
        throw e;
    }
	// cout << "left coordinate: " << left_avg << ", right coordinate: " << right_avg << endl;
	//step2 calculate camera values
	int middle_point = 0;
	int middle_err = 0;
	camearaRingTargetInfo new_target;
	new_target.located = true;
	new_target.distance_twoCam = -1.0f;
	new_target.distance_vertical = -1.0f;
	new_target.valid_twoCam = false;
	new_target.valid_vertical = false;
	switch (ir_state) {
		case cameraRingReadResultLost:
			timespec tmp;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);
			timespec diff;
			if ((tmp.tv_nsec - target_last_seen_time.tv_nsec) < 0) {
				diff.tv_sec = tmp.tv_sec - target_last_seen_time.tv_sec - 1;
				diff.tv_nsec = 1000000000 + tmp.tv_nsec - target_last_seen_time.tv_nsec;
			} else {
				diff.tv_sec = tmp.tv_sec - target_last_seen_time.tv_sec;
				diff.tv_nsec = tmp.tv_nsec - target_last_seen_time.tv_nsec;
			}
			// timespec diff time_diff(tmp, target_last_seen_time);
			if (diff.tv_sec != 0 || diff.tv_nsec >= 300000000) {
				searchState = cameraRingState_seeking;
				// current_active_pair = IRSensorPairInvalid;
				servo.set_tolerance(SERVO_SEEK_TOLERANCE);
				// servo_current_position = 0;
				// current_lower_bound = current_upper_bound = servo_current_position;
				return new_target;
			}
			return lastSeenTarget;
		case cameraRingReadResultBlobOnLeft:
			middle_point = -left_avg.X;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen_time);
			new_target.angle = servo.current_position();
			new_target.distance_twoCam = -1.0f;
			new_target.valid_twoCam = false;
			break;
		case cameraRingReadResultBlobOnRight:
			middle_point = FULL_HORIZONTAL - right_avg.X;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen_time);
			new_target.angle = servo.current_position();
			new_target.distance_twoCam = -1.0f;
			new_target.valid_twoCam = false;
			break;
		case cameraRingReadResultMiddle:
			double target_distance = calculate_target_coordinate(left_avg.X, right_avg.X);
			middle_point = FULL_HORIZONTAL - right_avg.X - left_avg.X;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &target_last_seen_time);
			new_target.angle = servo.current_position();
			new_target.distance_twoCam = target_distance;
			new_target.valid_twoCam = true;
		break;
	}

	if (following_pair == cameraRingSensorPairBack) {
		new_target.angle += 180;
	}

	lastSeenTarget = new_target;

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
	if (following_pair == cameraRingSensorPairFront) {
		correct = int(lround(PID_kernel(pid_front, middle_err, middle_point)));
	} else if (following_pair == cameraRingSensorPairBack) {
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
	int servo_current_position = servo.current_position() - correct;
	// servo_current_position = (int)servo.current_position();
	//add error output to servo
	// if (servo_current_position < 0) {
	// 	servo_current_position = 0;
	// 	searchState = cameraRingState_reversing;
	// 	should_reverse = true;
	// }
	// if (servo_current_position > 180) {
	// 	servo_current_position = 180;
	// 	searchState = cameraRingState_reversing;
	// 	should_reverse = true;
	// }
	servo.move_to(servo_current_position);

	return new_target;
}

bool cameraRing::targetLocked() {
	return searchState == cameraRingState_targetFound;
}

/**
 * @brief Queue Operation start here
 */

void cameraRing::encodeOprations() {
	//When encoding the sweep information to servo movement, everything should be negated, angle should be 180 - (angle) and from angle_high to angle_low
	switch (seekingState) {
		case cameraRingSeeking_notSeeking:
			break;
		case cameraRingSeeking_back: {
			cameraRingOperation newOp;
			newOp.pair = cameraRingSensorPairBack;
			newOp.low = back_angle_low;
			newOp.high = back_angle_high;
			newOp.isHead = true;
			// cout << "Encoding servo from " << newOp.low << " to " << newOp.high << " with back" << endl;
			operationQueue.push(newOp);
			break;
		}
		case cameraRingSeeking_front: {
			cameraRingOperation newOp;
			newOp.pair = cameraRingSensorPairFront;
			newOp.low = front_angle_low;
			newOp.high = front_angle_high;
			newOp.isHead = true;
			// cout << "Encoding servo from " << newOp.low << " to " << newOp.high << " with front" << endl;
			operationQueue.push(newOp);
			break;
		}
		case cameraRingSeeking_backFront: {
			cameraRingOperation backOp;
			backOp.pair = cameraRingSensorPairBack;
			backOp.low = back_angle_low;
			backOp.high = back_angle_high;
			backOp.isHead = true;
			// cout << "Encoding servo from " << backOp.low << " to " << backOp.high << " with back" << endl;
			operationQueue.push(backOp);

			cameraRingOperation frontOp;
			frontOp.pair = cameraRingSensorPairFront;
			frontOp.low = front_angle_low;
			frontOp.high = front_angle_high;
			frontOp.isHead = false;
			// cout << "Encoding servo from " << frontOp.low << " to " << frontOp.high << " with front" << endl;
			operationQueue.push(frontOp);
			break;
		}
		case cameraRingSeeking_frontBack: {
			cameraRingOperation frontOp;
			frontOp.pair = cameraRingSensorPairFront;
			frontOp.low = front_angle_low;
			frontOp.high = front_angle_high;
			frontOp.isHead = true;
			// cout << "Encoding servo from " << frontOp.low << " to " << frontOp.high << " with front" << endl;
			operationQueue.push(frontOp);

			cameraRingOperation backOp;
			backOp.pair = cameraRingSensorPairBack;
			backOp.low = back_angle_low;
			backOp.high = back_angle_high;
			backOp.isHead = false;
			// cout << "Encoding servo from " << backOp.low << " to " << backOp.high << " with back" << endl;
			operationQueue.push(backOp);
			break;
		}
		default:
			cerr << "cameraRing::encodeOprations::Invalid seeking state received: " << seekingState << endl;
			break;
	}
}

void cameraRing::shortenQueue() {
	if (operationQueue.size() > 2) {
		//means multiple singular or compound operations are in there
		queue<cameraRingOperation> newQueue;
		newQueue.push(operationQueue.front());
		operationQueue.pop();
		newQueue.push(operationQueue.front());
		// operationQueue.pop();
		swap(operationQueue, newQueue);
	}

	if (operationQueue.size() == 2) {
		if (operationQueue.back().isHead) {
			//Means second element is a standalone singular element
			queue<cameraRingOperation> newQueue;
			newQueue.push(operationQueue.front());
			swap(operationQueue, newQueue);
		}
	}
}

void cameraRing::purgeQueue() {
	queue<cameraRingOperation> emptyQueue;
	swap(operationQueue, emptyQueue);
}

/**
 * @brief Converter of angles.
 * @details [long description]
 *
 * @param publicAngle [description]
 * @return [description]
 */

inline int cameraRing::publicAngleToServoAngle(int publicAngle) {
	publicAngle -= SERVO_ANGLE_BIAS;
	if (publicAngle < SPHERE_DEGREE_ZERO) {
		publicAngle += SPHERE_DEGREE_FULL;
	} else if (publicAngle > SPHERE_DEGREE_FULL) {
		publicAngle -= SPHERE_DEGREE_FULL;
	}
	return publicAngle;
}

inline int cameraRing::servoAngleToPublicAngle(int servoAngle) {
	servoAngle += SERVO_ANGLE_BIAS;
	if (servoAngle < SPHERE_DEGREE_ZERO) {
		servoAngle += SPHERE_DEGREE_FULL;
	} else if (servoAngle > SPHERE_DEGREE_FULL) {
		servoAngle -= SPHERE_DEGREE_FULL;
	}
	return servoAngle;
}

void cameraRing::reset() {
	mux.reset();
}







/**
 * @brief Camera Ring Test start here
 */

cameraRingTestCamera::cameraRingTestCamera(gpioName muxResetPin, uint8_t channel) :
	mux(muxResetPin),
	testUnit()
{
	mux.selectChannel(1 << channel);
	testUnit.init();
}

void cameraRingTestCamera::testChannel() {
	uint8_t result = testUnit.readBlob();
	if (result & BLOB1) {
		cout << testUnit.Blob1 << endl;
	}
}


#include "cameraRing.h"

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
	servo_current_target(0),
	servo_current_edge_is_first(true),
	lastSeenTarget{NAN, NAN, false, false},
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

	cout << "In servo coordinate, low = " << angle_low << " high = " << angle_high << endl;

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
	cout << "fl: " << front_angle_low << " fh: " << front_angle_high << " bl: " << back_angle_low << " bh: " << back_angle_high << endl;
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
	if (servo.target_position_reached()) {
		if (servo_current_edge_is_first) {
			servo_current_edge_is_first = false;
			cameraRingOperation currentOp = operationQueue.front();
			servo_current_target = currentOp.high;
			cout << "Moving servo to " << servo_current_target << " with " << currentOp.pair << endl;

		} else {
			operationQueue.pop();
			servo_current_edge_is_first = true;
			if (operationQueue.empty()) {
				//Generate repetitive actions
				encodeOprations();
			}
			cameraRingOperation currentOp = operationQueue.front();
			servo_current_target = currentOp.low;
			cout << "Moving servo to " << servo_current_target << " with " << currentOp.pair << endl;

		}
	}
	servo.move_to(servo_current_target);
}

cameraRingReadResult cameraRing::readCamera(cameraRingSensorPair pair) {
	switch (pair) {
		case cameraRingSensorPairFront: {
			uint8_t leftResult = retrieveCameraResult(cameraRingCameraFrontLeft);
			uint8_t rightResult = retrieveCameraResult(cameraRingCameraFrontRight);
			Blob avgLeft = average(leftResult, &frontLeft);
			Blob avgRight = average(rightResult, &frontRight);

			if (Blob_is_valid(avgLeft) || Blob_is_valid(avgRight)) {
				if (Blob_is_valid(avgLeft) && Blob_is_valid(avgRight)) {
					return IRReadResultMiddle;
				} else if (Blob_is_valid(avgLeft)) {
					return IRReadResultBlobOnLeft;
				} else if (Blob_is_valid(avgRight)) {
					return IRReadResultBlobOnRight;
				}
			}
			return IRReadResultLost;
		}
		case cameraRingSensorPairBack: {
			uint8_t leftResult = retrieveCameraResult(cameraRingCameraBackLeft);
			uint8_t rightResult = retrieveCameraResult(cameraRingCameraBackRight);
			break;
		}
		default:
		case cameraRingSensorPairInvalid:
			cerr << "cameraRing::readCamera::Invalid sensor pairs received: " << pair << endl;
			break;
	}
}

uint8_t cameraRing::retrieveCameraResult(cameraRingCamera camIndex) {
	uint8_t result;
	try {
		switch (camIndex) {
			case cameraRingCameraFrontLeft:
				result = frontLeft.readBlob();
				break;
			case cameraRingCameraFrontRight:
				result = frontRight.readBlob();
				break;
			case cameraRingCameraBackLeft:
				result = backLeft.readBlob();
				break;
			case cameraRingCameraBackRight:
				result = backRight.readBlob();
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

void cameraRing::follow() {
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
			cout << "Encoding servo from " << newOp.low << " to " << newOp.high << " with back" << endl;
			operationQueue.push(newOp);
			break;
		}
		case cameraRingSeeking_front: {
			cameraRingOperation newOp;
			newOp.pair = cameraRingSensorPairFront;
			newOp.low = front_angle_low;
			newOp.high = front_angle_high;
			newOp.isHead = true;
			cout << "Encoding servo from " << newOp.low << " to " << newOp.high << " with front" << endl;
			operationQueue.push(newOp);
			break;
		}
		case cameraRingSeeking_backFront: {
			cameraRingOperation backOp;
			backOp.pair = cameraRingSensorPairBack;
			backOp.low = back_angle_low;
			backOp.high = back_angle_high;
			backOp.isHead = true;
			cout << "Encoding servo from " << backOp.low << " to " << backOp.high << " with back" << endl;
			operationQueue.push(backOp);

			cameraRingOperation frontOp;
			frontOp.pair = cameraRingSensorPairFront;
			frontOp.low = front_angle_low;
			frontOp.high = front_angle_high;
			frontOp.isHead = false;
			cout << "Encoding servo from " << frontOp.low << " to " << frontOp.high << " with front" << endl;
			operationQueue.push(frontOp);
			break;
		}
		case cameraRingSeeking_frontBack: {
			cameraRingOperation frontOp;
			frontOp.pair = cameraRingSensorPairFront;
			frontOp.low = front_angle_low;
			frontOp.high = front_angle_high;
			frontOp.isHead = true;
			cout << "Encoding servo from " << frontOp.low << " to " << frontOp.high << " with front" << endl;
			operationQueue.push(frontOp);

			cameraRingOperation backOp;
			backOp.pair = cameraRingSensorPairBack;
			backOp.low = back_angle_low;
			backOp.high = back_angle_high;
			backOp.isHead = false;
			cout << "Encoding servo from " << backOp.low << " to " << backOp.high << " with back" << endl;
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


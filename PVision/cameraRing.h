#ifndef CAMERA_RING_H
#define CAMERA_RING_H

#include <iostream>
#include <queue>
#include <ctime>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "PVision.h"
#include "PCA9548A.h"
#include "../BlackLib/BlackCore.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackADC.h"
#include "../BlackServo/BlackServo.h"
#include "../naughtyException/naughtyException.h"

using namespace std;
using namespace BlackLib;

typedef struct {
	float distance_twoCam;
	float distance_vertical;
	float angle;
	bool valid_twoCam;
	bool valid_vertical;
	bool located;
} camearaRingTargetInfo;

enum cameraRingSensorPair {
	cameraRingSensorPairFront,
	cameraRingSensorPairBack,
	cameraRingSensorPairInvalid
};

enum cameraRingCamera {
	cameraRingCameraFrontLeft,
	cameraRingCameraFrontRight,
	cameraRingCameraBackLeft,
	cameraRingCameraBackRight
};

/**
 * An operation might be either a head or not, a singular operation is always head
 */
typedef struct {
	cameraRingSensorPair pair;
	int low;
	int high;
	bool isHead;
} cameraRingOperation;

enum cameraRingReadResult {
	cameraRingReadResultLost,
	cameraRingReadResultBlobOnLeft,
	cameraRingReadResultBlobOnRight,
	cameraRingReadResultMiddle
};

enum cameraRingState {
	cameraRingState_seeking,
	cameraRingState_targetFound,
	cameraRingState_reversing,
	cameraRingState_stopped
};

enum cameraRingSeekingState {
	cameraRingSeeking_notSeeking,
	cameraRingSeeking_front,
	cameraRingSeeking_back,
	cameraRingSeeking_frontBack,
	cameraRingSeeking_backFront
};

class cameraRing {
public:
	cameraRing(pwmName servoPin, gpioName muxResetPin, adcName feedbackPin);
	void scanRange(int angle_low, int angle_high);
	void run();
	bool targetLocked();
	void reset();
	cameraRingReadResult readCamera(cameraRingSensorPair pair, Blob* left_avg = NULL, Blob* right_avg = NULL);
private:
	PCA9548A mux;
	PVision frontLeft;
	PVision frontRight;
	PVision backLeft;
	PVision backRight;
	BlackServo servo;
	//Those two values in servo coord system and only from 0 - 180 degrees
	int front_angle_low;
	int front_angle_high;
	int back_angle_low;
	int back_angle_high;
	cameraRingSensorPair following_pair;

	bool servo_current_edge_is_first;
	int servo_current_target;

	timespec target_last_seen_time;

	camearaRingTargetInfo lastSeenTarget;

	cameraRingState searchState;
	cameraRingSeekingState seekingState;
	queue<cameraRingOperation> operationQueue;


	void seek();
	camearaRingTargetInfo follow();
	uint8_t retrieveCameraResult(cameraRingCamera camIndex);
	inline int publicAngleToServoAngle(int publicAngle);
	inline int servoAngleToPublicAngle(int servoAngle);
	void encodeOprations();
	void shortenQueue();
	void purgeQueue();
	// float rangeOverlayPercent(int oldLow, int oldHigh, int newLow, int newHigh);
};

class cameraRingTestCamera {
public:
	cameraRingTestCamera(gpioName muxResetPin, uint8_t channel);
	void testChannel();
private:
	PCA9548A mux;
	PVision testUnit;
};

#endif

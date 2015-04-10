#include "DualStepperMotor.h"
#include <ctime>

#include <iostream>
using namespace std;

DualStepperMotor::DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight
	) :
	leftStepper(directionLeft, frequencyLeft),
	rightStepper(directionRight, frequencyRight) {

}

DualStepperMotor::~DualStepperMotor() {
	leftStepper.halt();
	rightStepper.halt();
}

//Public functions


void DualStepperMotor::moveForward(uint32_t speed) {
	leftStepper.run(1, speed);
	rightStepper.run(0, speed);
}

void DualStepperMotor::moveBackward(uint32_t speed) {
	leftStepper.run(0, speed);
	rightStepper.run(1, speed);
}

void DualStepperMotor::leftSpin(uint32_t speed) {
	leftStepper.run(0, speed);
	rightStepper.run(0, speed);
}

void DualStepperMotor::rightSpin(uint32_t speed) {
	leftStepper.run(1, speed);
	rightStepper.run(1, speed);
}

/**
 * @brief Make the steppers reach same speed at the same time
 */
// #define MATCHING_ACCEL

void DualStepperMotor::setAcceleration(uint16_t acceration_step) {
	#ifdef MATCHING_ACCEL
	int left_freq_diff = leftStepper.freq_diff();
	int right_freq_diff = rightStepper.freq_diff();
	if (left_freq_diff < right_freq_diff) {
		rightStepper.setAcceleration(acceration_step);

	} else {
		leftStepper.setAcceleration(acceration_step);

	}
	#else
	leftStepper.setAcceleration(acceration_step);
	rightStepper.setAcceleration(acceration_step);
	#endif
}

void DualStepperMotor::setBias(int bias) {
	// cout << "dual stepper setting bias" << endl;
	turn_bias = bias;
	leftStepper.setBias(bias);
	rightStepper.setBias(-bias);
}

void DualStepperMotor::run() {
	leftStepper.run();
	rightStepper.run();
}

void DualStepperMotor::stop() {
	leftStepper.stop();
	rightStepper.stop();
}

bool DualStepperMotor::targetSpeedReached() {
	bool leftReached = leftStepper.targetSpeedReached();
	bool rightReached = rightStepper.targetSpeedReached();

	// static clock_t left = -1, right = -1;

	// if (leftReached && (left == -1)) {
	// 	left = clock();
	// }
	// if (rightReached && (right == -1)) {
	// 	right = clock();
	// }

	// if (left != -1 && right != -1) {
	// 	cout << "Left finish clicks: " << left << ", times: " << ((float)left)/CLOCKS_PER_SEC << " seconds" << endl;
	// 	cout << "Right finish clicks: " << right << ", times: " << ((float)right)/CLOCKS_PER_SEC << " seconds" << endl;
	// }

	// return (leftStepper.targetSpeedReached() && rightStepper.targetSpeedReached());
	return leftReached && rightReached;
}

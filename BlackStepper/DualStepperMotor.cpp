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
	setAcceleration(20);
	moveForward(10000);
	while (!targetSpeedReached()) {
		run();
	}
}

//Public functions


void DualStepperMotor::moveForward(uint64_t speed) {
	leftStepper.run(1, speed);
	rightStepper.run(0, speed);
}

void DualStepperMotor::moveBackward(uint64_t speed) {
	leftStepper.run(0, speed);
	rightStepper.run(1, speed);
}

void DualStepperMotor::setAcceleration(uint16_t acceration_step) {
	leftStepper.setAcceleration(acceration_step);
	rightStepper.setAcceleration(acceration_step);
}

void DualStepperMotor::setBias(float bias) {
	turn_bias = bias;
	leftStepper.setBias(-bias);
	rightStepper.setBias(bias);
}

void DualStepperMotor::run() {
	leftStepper.run();
	rightStepper.run();
}

void DualStepperMotor::stop() {
	leftStepper.run(1, 0);
	rightStepper.run(0, 0);
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

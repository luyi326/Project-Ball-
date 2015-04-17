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
	leftStepper.run(0, 100);
	rightStepper.run(1, 100);
}

//Public functions


void DualStepperMotor::moveForward(unsigned int frequency) {
	leftStepper.run(1, frequency);
	rightStepper.run(0, frequency);
}

void DualStepperMotor::moveBackward(unsigned int frequency) {
	leftStepper.run(0, frequency);
	rightStepper.run(1, frequency);
}

// void DualStepperMotor::leftSpin(unsigned int frequency) {
// 	leftStepper.run(0, frequency);
// 	rightStepper.run(0, frequency);
// }

// void DualStepperMotor::rightSpin(unsigned int frequency) {
// 	leftStepper.run(1, frequency);
// 	rightStepper.run(1, frequency);
// }


void DualStepperMotor::setAcceleration(unsigned int acceration_step) {
	leftStepper.setAcceleration(acceration_step);
	rightStepper.setAcceleration(acceration_step);
}

void DualStepperMotor::setBias(int bias) {
	turn_bias = bias;
	leftStepper.setBias(-bias);
	rightStepper.setBias(bias);
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

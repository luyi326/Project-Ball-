#include "DualStepperMotor.h"

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

}

//Public functions


void DualStepperMotor::moveForward(uint64_t speed) {
	leftStepper.run(0, speed);
	rightStepper.run(1, speed);
}

void DualStepperMotor::moveBackward(uint64_t speed) {
	leftStepper.run(1, speed);
	rightStepper.run(0, speed);
}

void DualStepperMotor::turnLeft() {

}

void DualStepperMotor::turnRight() {

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
	return (leftStepper.targetSpeedReached() && rightStepper.targetSpeedReached());
}
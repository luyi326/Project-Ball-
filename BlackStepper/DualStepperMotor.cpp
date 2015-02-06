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
	rightStepper(directionRight, frequencyRight),
	currentDirection(1),
	currentSpeed(10000)
	 {

}

DualStepperMotor::~DualStepperMotor() {

}

//Public functions


void DualStepperMotor::moveForward(uint64_t speed) {
	currentDirection = 1;
	currentSpeed = speed;
	leftStepper.run(1, speed + turnBias);
	rightStepper.run(0, speed - turnBias);
	cout << "Left target: " << speed + turnBias << " right target: " << speed - turnBias << endl;
}

void DualStepperMotor::moveBackward(uint64_t speed) {
	currentDirection = 0;
	currentSpeed = speed;
	leftStepper.run(0, speed - turnBias);
	rightStepper.run(1, speed + turnBias);
}

void DualStepperMotor::setBias(int16_t bias) {
	turnBias = bias;
	cout << "turn bias = " << turnBias << endl;
	if (currentDirection) {
		this->moveForward(currentSpeed);
	} else {
		this->moveBackward(currentSpeed);
	}
}

void DualStepperMotor::run() {
	leftStepper.run();
	rightStepper.run();
}

void DualStepperMotor::stop() {
	currentDirection = 1;
	currentSpeed = 10000;
	leftStepper.run(1, 0);
	rightStepper.run(0, 0);
}

bool DualStepperMotor::targetSpeedReached() {
	bool leftReached = leftStepper.targetSpeedReached();
	bool rightReached = rightStepper.targetSpeedReached();
	if (leftReached && rightReached) cout << "Left speed: " << leftStepper.getSpeed() << " Right speed: " << rightStepper.getSpeed() << endl;
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
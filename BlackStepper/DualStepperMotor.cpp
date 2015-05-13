#include "DualStepperMotor.h"
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

#define ZERO_ERR (0.0000001)
#define LEVEL (-2.1)

DualStepperMotor::DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight,
		spiName kalman_spi_name,
		gpioName kalman_reset_pin_name
	) :
	leftStepper(directionLeft, frequencyLeft),
	rightStepper(directionRight, frequencyRight),
	kalduino(kalman_spi_name, kalman_reset_pin_name),
	pid(0.5, 1.0f / 40, 1.0f / 80, 20, -20),
	turn_bias(0),
	row_adjust (0) {
	float test_x_val = NAN;
	while (isnan(test_x_val) || abs(test_x_val) < ZERO_ERR) {
		test_x_val = kalduino.angleInfomation(arduinoConnector_KalmanX);
	}
}

DualStepperMotor::~DualStepperMotor() {
	leftStepper.run(0, 100);
	rightStepper.run(1, 100);
}

void DualStepperMotor::adjustBalance() {
	float degree = kalduino.angleInfomation(arduinoConnector_KalmanX);
	float kernel = pid.(degree - LEVEL, degree);
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
	cout << kalduino.angleInfomation(arduinoConnector_KalmanX) << endl;
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

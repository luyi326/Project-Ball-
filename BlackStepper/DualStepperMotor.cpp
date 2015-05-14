#include "DualStepperMotor.h"
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

#define ZERO_ERR (0.000000001)
#define LEVEL (-2.64081)

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
	pid(0.8, 0.0f, 0.0f, 200, -200),
	turn_bias(0),
	roll_adjust(0) {
	uint8_t i = 0;
	while (i < 3) {
		float test_x_val = NAN;
		int counter = 0;
		while ((isnan(test_x_val) || abs(test_x_val) < ZERO_ERR) && counter <= 100000) {
			test_x_val = kalduino.angleInfomation(arduinoConnector_KalmanX);
			counter++;
			if (!(isnan(test_x_val) || abs(test_x_val) < ZERO_ERR)) {
				//Init succeed
				return;
			}
		}
		kalduino.reset();
	}
	cerr << "Cannot initialize arduino in 3 tries" << endl;
	exit(1);
}

DualStepperMotor::~DualStepperMotor() {
	leftStepper.run(0, 100);
	rightStepper.run(1, 100);
}

void DualStepperMotor::adjustBalance() {
	cout << "Adjusting balance" << endl;
	float degree = kalduino.angleInfomation(arduinoConnector_KalmanX) - LEVEL;
	float error = 0.0f;
	int threashold = 2;
	if (degree - LEVEL > threashold) {
		error = degree - LEVEL - threashold;
	} else if (degree - LEVEL < -threashold) {
		error = degree - LEVEL + threashold;
	} else {
		error = 0.0f;
	}
	float kernel = pid.kernel(error, degree);
	cout << "Degree: " << degree << " error: " << error << " kernel: " << kernel << endl;
	roll_adjust = kernel;
	leftStepper.setBias(-turn_bias - roll_adjust);
	rightStepper.setBias(turn_bias + roll_adjust);
}

//Public function


void DualStepperMotor::moveForward(unsigned int frequency) {
	// Skip freq from -100 to 100
	if (frequency == 0) {
		frequency = 100;
	} if (frequency > 0) {
		frequency += 100;
	} else if (frequency < 0) {
		frequency -= 100;
	}
	adjustBalance();
	leftStepper.run(1, frequency);
	rightStepper.run(0, frequency);
}

void DualStepperMotor::moveBackward(unsigned int frequency) {
	if (frequency == 0) {
		frequency = 100;
	} if (frequency > 0) {
		frequency += 100;
	} else if (frequency < 0) {
		frequency -= 100;
	}
	adjustBalance();
	leftStepper.run(0, frequency);
	rightStepper.run(1, frequency);
}


void DualStepperMotor::setAcceleration(unsigned int acceration_step) {
	adjustBalance();
	leftStepper.setAcceleration(acceration_step);
	rightStepper.setAcceleration(acceration_step);
}

void DualStepperMotor::setBias(int bias) {
	turn_bias = bias;
	adjustBalance();
	// leftStepper.setBias(-bias);
	// rightStepper.setBias(bias);
}

void DualStepperMotor::run() {
	adjustBalance();
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
	return leftReached && rightReached;
}

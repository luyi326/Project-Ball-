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
		gpioName kalman_reset_pin_name,
		float pGain,
		float iGain,
		float dGain
	) :
	leftStepper(directionLeft, frequencyLeft),
	rightStepper(directionRight, frequencyRight),
	kalduino(kalman_spi_name, kalman_reset_pin_name),
	pid(pGain, iGain, dGain, 50, 0),
	turn_bias(0),
	current_direction(true),
	current_frequency(0),
	roll_adjust(0) {
	uint8_t i = 0;
	while (i < 3) {
		float test_x_val = NAN;
		int counter = 0;
		cout << "Initializing IMU..." << flush;
		while ((isnan(test_x_val) || abs(test_x_val) < ZERO_ERR) && counter <= 10000) {
			test_x_val = kalduino.angleInfomation(arduinoConnector_KalmanX);
			// cout << counter << " " <<  test_x_val << endl;
			counter++;
			if (!(isnan(test_x_val) || abs(test_x_val) < ZERO_ERR)) {
				//Init succeed
				cout << " Done" << endl;
				return;
			}
		}
		kalduino.reset();
		cout << "Failed" << endl;
		// cout << "Try resetting arduino" << endl;
		i++;
	}
	cerr << "Cannot initialize arduino in 3 tries" << endl;
	exit(1);
}

DualStepperMotor::~DualStepperMotor() {
	leftStepper.run(0, 100);
	rightStepper.run(1, 100);
}

void DualStepperMotor::adjustBalance(bool direction, unsigned frequency) {
	// cout << "Adjusting balance" << endl;
	float degree = kalduino.angleInfomation(arduinoConnector_KalmanX) - LEVEL;
	float error = 0.0f;
	float target = frequency / -200.0f;

	int threashold = 2;
	if (degree - target > threashold) {
		error = degree - threashold;
	} else if (degree + target < -threashold) {
		error = degree + threashold;
	} else {
		error = 0.0f;
	}
	float kernel = pid.kernel(error, degree);
	if (kernel<5 && kernel >-5){
		kernel = 0;
	}
	// cout << "Degree: " << degree << " error: " << error << " kernel: " << kernel << endl;
	roll_adjust = kernel;
	// cout << "turn bias is now " << turn_bias << " roll adjust is " << roll_adjust << endl;
	// cout << "left is biased at " << int(- float(turn_bias) + roll_adjust) << " right is biased at " << int(float(turn_bias) + roll_adjust) << endl;
	leftStepper.setBias(int(- float(turn_bias) + roll_adjust));
	rightStepper.setBias(int(float(turn_bias) + roll_adjust));
}

//Public function


void DualStepperMotor::moveForward(unsigned int frequency) {
	adjustBalance(true, frequency);
	current_direction = true;
	current_frequency = frequency;
	// Skip freq from -100 to 100
	if (frequency == 0) {
		frequency = 100;
	} else if (frequency > 0) {
		frequency += 100;
	}
	// cout << "running at freq " << frequency << endl;
	leftStepper.run(1, frequency);
	rightStepper.run(0, frequency);
}

void DualStepperMotor::moveLeft(int angle) {
	// int bias = angle * 15;
	// cout << "Moving left 1" << endl;
	turn_bias = angle * 10;
	leftStepper.run(1, 2000);
	// cout << "Moving left 2" << endl;
	rightStepper.run(0, 3000);
	// cout << "Moving left 3" << endl;
	adjustBalance(true, 2000);
	// cout << "Moving left 4" << endl;
}

void DualStepperMotor::moveRight(int angle) {
	// int bias = angle * 15;
	turn_bias = - angle * 10;
	leftStepper.run(1, 3000);
	rightStepper.run(0, 2000);
	adjustBalance(true, 2000);
}

void DualStepperMotor::moveBackward(unsigned int frequency) {
	adjustBalance(false, frequency);
	current_direction = false;
	current_frequency = frequency;
	// Skip freq from -100 to 100
	if (frequency == 0) {
		frequency = 100;
	} else if (frequency > 0) {
		frequency += 100;
	}
	leftStepper.run(0, frequency);
	rightStepper.run(1, frequency);
}

void DualStepperMotor::setAcceleration(unsigned int acceration_step) {
	adjustBalance(current_direction, current_frequency);
	leftStepper.setAcceleration(acceration_step);
	rightStepper.setAcceleration(acceration_step);
}

void DualStepperMotor::setBias(int bias) {
	turn_bias = bias;
	adjustBalance(current_direction, current_frequency);
	// leftStepper.setBias(-bias);
	// rightStepper.setBias(bias);
}

void DualStepperMotor::run() {
	adjustBalance(current_direction, current_frequency);
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

#ifndef DUAL_STEPPER_MOTOR_H
#define DUAL_STEPPER_MOTOR_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include "BlackStepper.h"
#include "../PID/PID.h"
#include "../arduinoConnector/arduinoConnector.h"
using namespace BlackLib;

#define STEPPER_BALANCING_ON

class DualStepperMotor {
private:
	BlackStepper leftStepper;
	BlackStepper rightStepper;
	arduinoConnector kalduino;
	PID pid;
	int turn_bias;
	bool current_direction;
	unsigned current_frequency;
	float roll_adjust;

	void adjustBalance(bool direction, unsigned frequency);

public:
	DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight,
		spiName kalman_spi_name,
		gpioName kalman_reset_pin_name,
		float pGain,
		float iGain,
		float dGain
	);

	void moveForward(unsigned int frequency);
	void moveBackward(unsigned int frequency);
	void moveLeft(int angle);
	void moveRight(int angle);
	void setAcceleration(unsigned int acceration_step);
	void run();
	void setBias(int bias);
	void stop();

	bool targetSpeedReached();

	~DualStepperMotor();
};

#endif
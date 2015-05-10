#ifndef DUAL_STEPPER_MOTOR_H
#define DUAL_STEPPER_MOTOR_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include "BlackStepper.h"
#include "../PID/PID.h"
#include "../arduinoConnector.h"
using namespace BlackLib;

#define STEPPER_BALANCING_ON

class DualStepperMotor {
private:
	BlackStepper leftStepper;
	BlackStepper rightStepper;
	arduinoConnector kalduino;
	PID pid;
	unsigned int turn_bias;

public:
	DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight,
		spiName kalman_spi_name,
		gpioName kalman_reset_pin_name
	);

	void moveForward(unsigned int frequency);
	void moveBackward(unsigned int frequency);
	void setAcceleration(unsigned int acceration_step);
	void run();
	void setBias(int bias);
	void stop();

	bool targetSpeedReached();

	~DualStepperMotor();
};

#endif
#ifndef DUAL_STEPPER_MOTOR_H
#define DUAL_STEPPER_MOTOR_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include "BlackStepper.h"
using namespace BlackLib;

class DualStepperMotor {
private:
	BlackStepper leftStepper;
	BlackStepper rightStepper;
	unsigned int turn_bias;

public:
	DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight
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
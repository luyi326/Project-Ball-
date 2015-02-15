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
	float turn_bias;

public:
	DualStepperMotor(
		gpioName directionLeft,
		pwmName frequencyLeft,
		gpioName directionRight,
		pwmName frequencyRight
	);

	void moveForward(uint64_t speed);
	void moveBackward(uint64_t speed);
	void run();
	void setBias(float bias);
	void stop();

	bool targetSpeedReached();

	~DualStepperMotor();
};

#endif
#ifndef DUAL_STEPPER_MOTOR_H
#define DUAL_STEPPER_MOTOR_H

#include <stdint.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include "BlackStepper.h"
using namespace BlackLib;

class DualStepperMotor {
private:
	BlackStepper leftStepper;
	BlackStepper rightStepper;
	int16_t turnBias;

	uint8_t currentDirection;
	uint64_t currentSpeed;

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
	void setBias(int16_t bias);
	void stop();

	bool targetSpeedReached();

	~DualStepperMotor();
};

#endif
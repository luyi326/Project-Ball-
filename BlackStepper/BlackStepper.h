#ifndef BLACK_STEPPER_H
#define BLACK_STEPPER_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
using namespace BlackLib;

class BlackStepper {
private:
	BlackGPIO _direction;
	BlackPWM _frequency;
	//_current_direction, 0 : forward, 1: backward
	bool _current_direction;
	//_current_speed refers to frequency
	uint64_t _current_speed;

	void setMovement(bool direction, uint64_t speed);
public:
	BlackStepper(gpioName direction, pwmName frequency);
	~BlackStepper();

	bool run(bool direction, uint64_t speed);
	bool stop();

};

#endif
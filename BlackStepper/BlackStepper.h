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
	uint64_t _current_freq;

	unsigned long _last_timestamp;

	bool _target_direction;
	uint64_t _target_speed;
	uint64_t _target_freq;

	bool _speedReached;

	void setMovement(bool direction, uint64_t speed);
	void setGPIOAndPWM(bool direction, uint64_t frequency);
	unsigned long micros();

public:
	BlackStepper(gpioName direction, pwmName frequency);
	~BlackStepper();

	void run(bool direction, uint64_t speed);
	void run();
	void stop();

	bool getDirection();
	uint64_t getSpeed();
	bool targetSpeedReached();

};

#endif
#ifndef BLACK_STEPPER_H
#define BLACK_STEPPER_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include <ctime>
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

	timespec _last_timestamp;

	bool _target_direction;
	uint64_t _target_speed;
	uint64_t _target_freq;
	uint16_t _current_accelration_step;

	float _turn_freq_bias;

	bool _speedReached;

	void setMovement(bool direction, uint64_t speed);
	void setGPIOAndPWM(bool direction, uint64_t frequency);
	inline bool isLongEnough();

public:
	BlackStepper(gpioName direction, pwmName frequency);
	~BlackStepper();

	void run(bool direction, uint64_t speed);
	void run();
	void stop();
	void halt();
	void setAcceleration(uint16_t acceration_step);
	void setBias(float bias);

	bool getDirection();
	uint64_t getSpeed();
	uint16_t getAcceleration();
	float getBias();
	bool targetSpeedReached();

};

#endif
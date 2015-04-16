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
	unsigned int _current_freq;

	timespec _last_timestamp;

	bool _target_direction;
	unsigned int  _target_freq;
	unsigned int _current_accelration_step;

	int _turn_freq_bias;

	bool _speedReached;

	void setMovement(bool direction, unsigned int frequency);
	void setGPIOAndPWM(bool direction, unsigned int frequency);
	inline bool isLongEnough();

public:
	BlackStepper(gpioName direction, pwmName frequency);
	~BlackStepper();

	void run(bool direction, unsigned int frequency);
	void run();
	void stop();
	void setAcceleration(unsigned int acceration_step);
	void setBias(int bias);

	bool getDirection();
	unsigned int getFrequency();
	unsigned int getAcceleration();
	int getBias();
	bool targetSpeedReached();

};

#endif
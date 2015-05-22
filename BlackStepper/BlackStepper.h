#ifndef BLACK_STEPPER_H
#define BLACK_STEPPER_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include <ctime>

#define PERIOD_MICRO_TO_FREQ(period) (1000000/period)
#define FREQ_TO_PERIOD_MICRO(freq) (1000000/freq)

#define PERIOD_MAX 10000
#define PERIOD_MIN 170

#define FREQ_MAX PERIOD_MICRO_TO_FREQ(PERIOD_MIN) // 5882
#define FREQ_MIN PERIOD_MICRO_TO_FREQ(PERIOD_MAX) // 100

#define SPEED_MAX (FREQ_MAX - 100)
#define SPEED_MIN (FREQ_MIN - 100)

#define BIAS_MAX 0.98

using namespace BlackLib;

class BlackStepper {
private:
	BlackGPIO _direction;
	BlackPWM _frequency;

	timespec _last_timestamp;

	int _target_speed;
	int _current_speed;
	int _last_accel;
	int _current_accelration_step;

	// int _turn_freq_bias;

	bool _speedReached;

	void setMovement(bool direction, unsigned int frequency);
	void setGPIOAndPWM(bool direction, unsigned int frequency);
	inline bool isLongEnough();

public:
	BlackStepper(gpioName direction, pwmName frequency);
	~BlackStepper();
	adjustSpeed(int speed);
	adjustSpeed();
	void stop();
	void setAcceleration(unsigned int acceration_step);

	unsigned int getAcceleration();
	bool targetSpeedReached();

};

#endif
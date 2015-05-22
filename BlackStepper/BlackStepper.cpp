#include <iostream>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include "BlackStepper.h"
using namespace std;

// #define STEPPER_DEBUG

#define STEP_INTERVAL 10000
// #define STEP_SIZE_FREQ 15
#define DEFAULT_ACCEL_STEP 15

/**
 * @brief Motor and ball hardware description
 */
#define DRIVER_REDUCTION (1/16.0f)
#define UNIT_MOTOR_STEP (1.8f)
#define REAL_MOTOR_STEP (DRIVER_REDUCTION*UNIT_MOTOR_STEP)
#define DEGREE_TO_RADIAN(degree) (0.017453f*degree)
#define ANGULAR_SPEED(freq) (freq*DEGREE_TO_RADIAN(REAL_MOTOR_STEP)) //Radian per second
#define LINEAR_SPEED(freq) (31.13f*ANGULAR_SPEED(freq)) // mm per second

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) :
_direction(direction, output, SecureMode),
_frequency(frequency) {
	_speedReached = 1;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_last_timestamp);
	_target_speed = 0;
	_current_speed = 0;
	_last_accel = 0;
	_nominal_last_accel = 0;
	_correction = 0;
	_current_accelration_step = DEFAULT_ACCEL_STEP;
	// _turn_freq_bias = 0;
	stop();
}

BlackStepper::~BlackStepper() {
	stop();
	usleep(200);
}

void BlackStepper::stop() {
	setSpeed(0);
}

void BlackStepper::setAcceleration(unsigned int acceration_step) {
	_current_accelration_step = acceration_step;
}

bool BlackStepper::targetSpeedReached() {
	return _speedReached;
}
unsigned int BlackStepper::getAcceleration() {
	return _current_accelration_step;
}

//Assume it always need less than 1s
inline bool BlackStepper::isLongEnough() {
	timespec temp_time;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp_time);
	if (temp_time.tv_sec - _last_timestamp.tv_sec != 0) {
		return true;
	}
	if (temp_time.tv_nsec - _last_timestamp.tv_nsec > STEP_INTERVAL) {
		return true;
	}
	return false;
}

float BlackStepper::getLinearAcceleration() {
	return float(_nominal_last_accel) / STEP_INTERVAL * 100.0f;
}

float BlackStepper::getLinearSpeed() {
	int realFreq = 0;
	if (_current_speed > 0) {
		realFreq = _current_speed + 100;
	} else if (-_current_speed < 0) {
		realFreq = _current_speed - 100;
	}
	return LINEAR_SPEED(realFreq);
}

void BlackStepper::adjustSpeed(int speed, int correction) {
	_target_speed = speed;
	_correction = correction;
	adjustSpeed(correction);
}

void BlackStepper::adjustSpeed(int correction) {
	// cout << "in adj speed" << endl;
	if (isLongEnough()) {
		int speed_diff = abs(_target_speed + correction - _current_speed);
		int nominal_speed_diff = abs(_target_speed  - _current_speed);
		// bool sameDirection = (_target_speed >= 0) ^ (_current_speed < 0);
		// cout << "speed to go " << speed_diff << endl;
		if (speed_diff == 0) {
			_speedReached = true;
			_last_accel = 0;
		} else if (speed_diff < _current_accelration_step) {
			_speedReached = false;
			_last_accel = speed_diff;

			if (_target_speed > _current_speed) {
				_current_speed += speed_diff;
			} else {
				_current_speed -= speed_diff;
			}
			setSpeed(_current_speed);
		} else {
			_speedReached = false;
			_last_accel = _current_accelration_step;

			if (_target_speed > _current_speed) {
				_current_speed += _current_accelration_step;
			} else {
				_current_speed -= _current_accelration_step;
			}
			setSpeed(_current_speed);
		}

		if (nominal_speed_diff == 0) {
			_nominal_last_accel = 0;
		} else if (nominal_speed_diff < _current_accelration_step) {
			_nominal_last_accel = nominal_speed_diff;
		} else {
			_nominal_last_accel = _current_accelration_step;
		}
		if (_target_speed < 0) {
			_nominal_last_accel = - _nominal_last_accel;
		}
		// cout << "New current speed is " << _current_speed << endl;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_last_timestamp);
	} else {

	}
}

/**
 * @brief Speed is from -5782 to 5782, will be extended by 100, -100 to 100 is the deadband
 * @details Convert "speed" to actual frequency
 *
 * @param speed [description]
 */
inline void BlackStepper::setSpeed(int speed) {
	if (speed > 0) {
		if (speed < SPEED_MIN) {
			speed = SPEED_MIN;
		} else if (speed > SPEED_MAX) {
			speed = SPEED_MAX;
		} else {
			speed += 100;
		}
	} else if (speed < 0) {
		if (speed > - SPEED_MIN) {
			speed = - SPEED_MIN;
		} else if (speed < - SPEED_MAX) {
			speed = - SPEED_MAX;
		} else {
			speed -= 100;
		}
	} else {
		speed = 100;
	}
	if (speed > 0) {
		// cout << "Setting speed to " << static_cast<unsigned int>(speed) << " direction to 0" << endl;
		setGPIOAndPWM(false, static_cast<unsigned int>(speed));
	} else {
		// cout << "Setting speed to " << static_cast<unsigned int>(-speed) << " direction to 1" << endl;
		setGPIOAndPWM(true, static_cast<unsigned int>(-speed));
	}
}

inline void BlackStepper::setGPIOAndPWM(bool direction, unsigned int frequency) {
	#ifdef STEPPER_DEBUG
	cout << "BS::setGPIOAndPWM::Setting direction " << (digitalValue)direction << " and frequency " << frequency << endl;
	#endif
	_direction.setValue((digitalValue)direction);
	if (frequency <= FREQ_MIN) {
		#ifdef STEPPER_DEBUG
		cout << "BS::setGPIOAndPWM::Period is too high, stop PWM" << endl;
		#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MAX, microsecond);
		_frequency.setDutyPercent(0.0);

	} else if (frequency >= FREQ_MAX) {
		#ifdef STEPPER_DEBUG
		cout << "BS::setGPIOAndPWM::Period is too low, set period to 170 micros" << endl;
		#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MIN, microsecond);
		_frequency.setDutyPercent(60.0);

	} else {
		#ifdef STEPPER_DEBUG
		cout << "BS::setGPIOAndPWM::Set period to " << FREQ_TO_PERIOD_MICRO(frequency) << " micros" << endl;
		#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(FREQ_TO_PERIOD_MICRO(frequency), microsecond);
		_frequency.setDutyPercent(50.0);
	}
	#ifdef STEPPER_DEBUG
	cout << endl;
	#endif
}

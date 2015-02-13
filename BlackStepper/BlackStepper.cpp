#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include "BlackStepper.h"
using namespace std;

// #define STEPPER_DEBUG

#define STEP_INTERVAL 5000
// #define STEP_SIZE_FREQ 15
#define DEFAULT_ACCEL_STEP 15

#define PERIOD_MAX 10000
#define PERIOD_MIN 170

#define PERIOD_MICRO_TO_FREQ(period) ((uint64_t)(1000000/period))
#define FREQ_TO_PERIOD_MICRO(freq) ((uint64_t)(1000000/freq))

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) : _direction(direction, output, SecureMode), _frequency(frequency) {
	_speedReached = 1;
	_last_timestamp = micros();

	_target_direction = _current_direction = 0;
	_target_speed = _current_speed = PERIOD_MAX;
	_target_freq = _current_freq = PERIOD_MICRO_TO_FREQ(_target_speed);
	_current_accelration_step = DEFAULT_ACCEL_STEP;
	_turn_freq_bias = 0;
	setGPIOAndPWM(0, 0);
}

BlackStepper::~BlackStepper() {
	setGPIOAndPWM(0, 0);
	usleep(200);
}

void BlackStepper::run(bool direction, uint64_t speed) {
	setMovement(direction, speed);
}

void BlackStepper::run() {
	run(_target_direction, _target_speed);
}

void BlackStepper::stop() {
	setMovement(0, 0);
}

void BlackStepper::setAcceleration(uint64_t acceration_step) {
	_current_accelration_step = acceration_step;
}

void BlackStepper::setBias(int16_t bias) {
	_turn_freq_bias = bias;
}

//Getters
bool BlackStepper::getDirection() {
	return _current_direction;
}

uint64_t BlackStepper::getSpeed() {
	return _current_speed;
}

bool BlackStepper::targetSpeedReached() {
	return _speedReached;
}
uint16_t BlackStepper::getAcceleration() {
	return _current_accelration_step;
}

int16_t getBias() {
	return _turn_freq_bias;
}

//Private functions
void BlackStepper::setMovement(bool direction, uint64_t speed) {
	_target_direction = direction;
	_target_speed = speed;
	if (speed > PERIOD_MAX) speed = PERIOD_MAX;
	if (speed < PERIOD_MIN) speed = PERIOD_MIN;
	_target_freq = (uint64_t)((int64_t)PERIOD_MICRO_TO_FREQ(speed) + _turn_freq_bias);

	_speedReached = 0;

	// If the target is speed already reached, just return. UNLESS some other program is modifying the gpio
	if (direction == _current_direction && speed == _current_speed) {
#ifdef STEPPER_DEBUG
		cout << "Target speed " << _target_speed << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
#endif
		_speedReached = 1;
		return;
	}


	if (micros() - _last_timestamp > STEP_INTERVAL) {
#ifdef STEPPER_DEBUG
		cout << "Updating motor motion, last timestamp: " << _last_timestamp << ", new timestamp: " << micros() << endl;
		cout << "Target direction: " << _target_direction << ", target speed: " << _target_speed << endl;
#endif
		int _cal_new_freq = (int)_current_freq;

		uint64_t real_step = _current_accelration_step;
		if (_current_direction == _target_direction) {
			real_step = std::min( ((uint64_t)std::abs(_cal_new_freq - _target_freq)), real_step);
#ifdef STEPPER_DEBUG
			cout << "real step size set to " << real_step << endl;
#endif
		}

#ifdef STEPPER_DEBUG
		cout << "Current direction: " << _current_direction << ", current speed: " << _current_speed << endl;
#endif

		if (_current_direction == _target_direction && (uint64_t)_cal_new_freq < _target_freq) {
			_cal_new_freq += (int)real_step;
		} else {
			_cal_new_freq -= (int)real_step;
		}

		if (_cal_new_freq < 0) {
			_cal_new_freq = - _cal_new_freq;
			_current_direction = !_current_direction;
		}

		_current_freq = _cal_new_freq;
		if (_cal_new_freq == 0) {
			_current_speed = PERIOD_MAX;
		} else {
			_current_speed = FREQ_TO_PERIOD_MICRO(_cal_new_freq);
		}

		if (_current_speed > PERIOD_MAX) _current_speed = PERIOD_MAX;
		if (_current_speed < PERIOD_MIN) _current_speed = PERIOD_MIN;

#ifdef STEPPER_DEBUG
		cout << "New direction: " << _current_direction << ", new speed: " << _current_speed << endl;
		cout << "Step size: " << real_step << endl;
#endif

		setGPIOAndPWM(_current_direction, (uint64_t)_cal_new_freq);

		if (_target_direction == _current_direction && _target_speed == _current_speed) {
#ifdef STEPPER_DEBUG
			cout << "Target speed " << _target_speed << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
#endif
			_speedReached = 1;
		}

		// update timestamp
		_last_timestamp = micros();
	}
}

inline unsigned long BlackStepper::micros() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

inline void BlackStepper::setGPIOAndPWM(bool direction, uint64_t frequency) {
#ifdef STEPPER_DEBUG
	cout << "Setting direction " << (digitalValue)direction << endl;
#endif
	_direction.setValue((digitalValue)direction);
	if ((frequency == 0) || (FREQ_TO_PERIOD_MICRO(frequency) >= PERIOD_MAX)) {
#ifdef STEPPER_DEBUG
		cout << "Period is too high, stop PWM" << endl;
#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MAX, microsecond);
		_frequency.setDutyPercent(0.0);

	} else if (FREQ_TO_PERIOD_MICRO(frequency) <= PERIOD_MIN) {
#ifdef STEPPER_DEBUG
		cout << "Period is too low, set period to 170 micros" << endl;
#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MIN, microsecond);
		_frequency.setDutyPercent(60.0);

	} else {
#ifdef STEPPER_DEBUG
		cout << "Set period to " << FREQ_TO_PERIOD_MICRO(frequency) << " micros" << endl;
#endif
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(FREQ_TO_PERIOD_MICRO(frequency), microsecond);
		_frequency.setDutyPercent(50.0);
}
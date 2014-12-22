#include <chrono>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include "BlackStepper.h"

#define STEP_INTERVAL 2000
#define STEP_SIZE_FREQ 100

#define PERIOD_MAX 10000
#define PERIOD_MIN 150

#define PERIOD_MICRO_TO_FREQ(period) ((uint64_t)(1000000/period))
#define FREQ_TO_PERIOD_MICRO(freq) ((uint64_t)(1000000/freq))

#include <iostream>
using namespace std;

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) : _direction(direction, output, SecureMode), _frequency(frequency) {
	_speedReached = 1;
	_last_timestamp = micros();
	_target_direction = _current_direction = 0;
	_target_speed = _current_speed = 0;

	// _direction.setValue((digitalValue)0);
	// _frequency.setDutyPercent(100.0);
	// _frequency.setPeriodTime(0, BlackLib::microsecond);
	// _frequency.setDutyPercent(60.0);
	// usleep(200);

	setMovement(_current_direction, _current_speed);
}

BlackStepper::~BlackStepper() {

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

//Private functions
void BlackStepper::setMovement(bool direction, uint64_t speed) {
	_target_direction = direction;
	_target_speed = speed;
	if (speed > PERIOD_MAX) speed = PERIOD_MAX;
	if (speed < PERIOD_MIN) speed = PERIOD_MIN;
	_target_freq = PERIOD_MICRO_TO_FREQ(speed);

	_current_speed = _frequency.getNumericPeriodValue() / 1000;
	_current_direction = (bool)_direction.getNumericValue();
	_speedReached = 0;

	// If the target is speed already reached, just return. UNLESS some other program is modifying the gpio
	if (direction == _current_direction && speed == _current_speed) {
		cout << "Target speed " << _target_speed << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
		_speedReached = 1;
		return;
	}


	if (micros() - _last_timestamp > STEP_INTERVAL) {
		cout << "Updating motor motion, last timestamp: " << _last_timestamp << ", new timestamp: " << micros() << endl;
		cout << "Target direction: " << _target_direction << ", target speed: " << _target_speed << endl;
		int64_t _cal_new_freq = (int64_t)PERIOD_MICRO_TO_FREQ(_current_speed);

		uint64_t real_step = STEP_SIZE_FREQ;
		if (_current_direction == _target_direction) {
			real_step = std::min( ((uint64_t)std::abs(_cal_new_freq - _target_freq)), (uint64_t)STEP_SIZE_FREQ);
		}

		cout << "Current direction: " << _current_direction << ", current speed: " << _current_speed << endl;

		if (_current_direction == _target_direction && (uint64_t)_cal_new_freq < _target_freq) {
			_cal_new_freq += real_step;
		} else {
			_cal_new_freq -= real_step;
		}

		if (_cal_new_freq < 0) {
			_cal_new_freq = - _cal_new_freq;
			_current_direction = !_current_direction;
		}

		_current_speed = FREQ_TO_PERIOD_MICRO(_cal_new_freq);

		cout << "New direction: " << _current_direction << ", new speed: " << _current_speed << endl;
		cout << "Step size: " << real_step << endl;

		setGPIOAndPWM(_current_direction, (uint64_t)_cal_new_freq);

		if (_target_direction == _current_direction && _target_speed == _current_speed) {
			cout << "Target speed " << _target_speed << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
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
	_direction.setValue((digitalValue)direction);
	if ((frequency == 0) || (FREQ_TO_PERIOD_MICRO(frequency) >= PERIOD_MAX)) {
		cout << "Period is too high, stop PWM" << endl;
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MAX, microsecond);
		_frequency.setDutyPercent(0.0);

	} else if (FREQ_TO_PERIOD_MICRO(frequency) <= PERIOD_MIN) {
		cout << "Period is too low, set period to 150 micros" << endl;
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(PERIOD_MIN, microsecond);
		_frequency.setDutyPercent(60.0);

	} else {
		cout << "Set period to " << FREQ_TO_PERIOD_MICRO(frequency) << " micros" << endl;
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(FREQ_TO_PERIOD_MICRO(frequency), microsecond);
		_frequency.setDutyPercent(60.0);
	}
	//Allow setup
	usleep(200);
}
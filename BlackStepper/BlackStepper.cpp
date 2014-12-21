#include <chrono>
#include <unistd.h>
#include "BlackStepper.h"

#define STEP_INTERVAL 2000
#define STEP_SIZE 50

#include <iostream>
using namespace std;

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) : _direction(direction, output, SecureMode), _frequency(frequency) {
	_speedReached = true;
	_last_timestamp = micros();
	setMovement(_current_direction, _current_speed);
}

BlackStepper::~BlackStepper() {

}

bool BlackStepper::run(bool direction, uint64_t speed) {
	setMovement(direction, speed);
}

bool BlackStepper::run() {
	run(_target_direction, _target_speed);
}

bool BlackStepper::stop() {
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

	//If the target is speed already reached, just return. UNLESS some other program is modifying the gpio
	if (direction == _current_direction && speed == _current_speed) {
		cout << "Target speed is reached, not doing anything" << endl;
		_speedReached = true;
		return;
	}

	_speedReached = false;

	if (micros() - _last_timestamp > STEP_INTERVAL) {
		cout << "Updating motor motion, last timestamp: " << _last_timestamp << ", new timestamp: " << micros() << endl;

		int64_t calculated_new_speed = (int64_t)_current_speed;
		calculated_new_speed = _current_direction == _target_direction ? calculated_new_speed + STEP_SIZE : calculated_new_speed - STEP_SIZE;
		//if direction is reversed, then flip current direction

		if (calculated_new_speed) {
			_current_direction = !_current_direction;
			calculated_new_speed = - calculated_new_speed;
			cout << "Last direction: " << -_current_direction << ", new direction: " << _current_direction << endl;
		} else {
			cout << "Last direction: " << _current_direction << ", new direction: " << _current_direction << endl;
		}
		//Constrain new speed to target speed, only when current direction and target direction are in alignment
		if (_current_direction == _target_direction && calculated_new_speed > _target_speed) {
			calculated_new_speed = _target_speed;
		}
		cout << "Last speed: " << _current_speed << ", new speed: " << calculated_new_speed << endl;

		//If target reached, set the _speedReached flag
		if (_current_direction == _target_direction && calculated_new_speed == _target_speed) {
			_speedReached = true;
			cout << "Speed reached" << endl;
		}

		_current_speed = calculated_new_speed;

		_direction.setValue((digitalValue)direction);
		_frequency.setDutyPercent(100.0);
		_frequency.setPeriodTime(speed, BlackLib::microsecond);
		_frequency.setDutyPercent(60.0);
		usleep(200);

	} else {
		cout << "Not updating, last timestamp: " << _last_timestamp << ", new timestamp: " << micros() << endl;
	}
	//update timestamp
	_last_timestamp = micros();
}

inline unsigned long BlackStepper::micros() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}
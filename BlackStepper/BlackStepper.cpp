#include "BlackStepper.h"

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) : _direction(direction, output, SecureMode), _frequency(frequency) {
	setMovement(_current_direction, _current_speed);
}

BlackStepper::~BlackStepper() {

}

bool BlackStepper::run(bool direction, uint64_t speed) {
	setMovement(direction, speed);
	return true;
}

bool BlackStepper::stop() {
	setMovement(0, 0);
	return true;
}

//Private functions
void BlackStepper::setMovement(bool direction, uint64_t speed) {
	_direction.setValue((digitalValue)direction);
    _frequency.setDutyPercent(100.0);
    _frequency.setPeriodTime(speed, BlackLib::microsecond);
    _frequency.setDutyPercent(60.0);
    sleep(1);
	_current_direction = direction;
	_current_speed = speed;
}
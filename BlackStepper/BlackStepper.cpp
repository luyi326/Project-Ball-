#include "BlackStepper.h"

//Public functions
BlackStepper::BlackStepper(gpioName direction, pwmName frequency) {
	_direction(direction, output, SecureMode);
	_frequency(frequency);
	setMovement(_current_direction, _current_speed);
}

BlackStepper::~BlackStepper() {

}

bool run(bool direction, uint64_t speed) {
	setMovement(direction, speed);
}

bool stop() {
	setMovement(0, 0);
}

//Private functions
void setMovement(bool direction, uint64_t speed) {
	_direction.setValue(direction);
    _frequency.setDutyPercent(100.0);
    _frequency.setPeriodTime(speed, BlackLib::microsecond);
    _frequency.setDutyPercent(60.0);
	_current_direction = direction;
	_current_speed = speed;
}
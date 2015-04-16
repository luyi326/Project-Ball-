#include <iostream>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include "BlackStepper.h"
using namespace std;

// #define STEPPER_DEBUG

#define STEP_INTERVAL 5000000
// #define STEP_SIZE_FREQ 15
#define DEFAULT_ACCEL_STEP 15

#define PERIOD_MICRO_TO_FREQ(period) (1000000/period)
#define FREQ_TO_PERIOD_MICRO(freq) (1000000/freq)

#define PERIOD_MAX 10000
#define PERIOD_MIN 170

#define FREQ_MAX PERIOD_MICRO_TO_FREQ(PERIOD_MIN) // 5882
#define FREQ_MIN PERIOD_MICRO_TO_FREQ(PERIOD_MAX) // 100

#define BIAS_MAX 0.98

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

	_target_direction = _current_direction = 0;
	// _target_speed = _current_speed = PERIOD_MAX;
	_target_freq = _current_freq = FREQ_MIN;
	_current_accelration_step = DEFAULT_ACCEL_STEP;
	_turn_freq_bias = 0;
	setGPIOAndPWM(0, 0);
}

BlackStepper::~BlackStepper() {
	setGPIOAndPWM(0, 0);
	usleep(200);
}

void BlackStepper::run(bool direction, unsigned int frequency) {
	setMovement(direction, frequency);
}

void BlackStepper::run() {
	run(_target_direction, _target_freq);
}

void BlackStepper::stop() {
	setMovement(0, PERIOD_MAX);
}

void BlackStepper::setAcceleration(unsigned int acceration_step) {
	_current_accelration_step = acceration_step;
}

void BlackStepper::setBias(unsigned int bias) {
	// if (bias > BIAS_MAX) {
	// 	bias = BIAS_MAX;
	// }
	// if (bias < - BIAS_MAX) {
	// 	bias = - BIAS_MAX;
	// }
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

float BlackStepper::getBias() {
	return _turn_freq_bias;
}

//Private functions
void BlackStepper::setMovement(bool direction, unsigned int frequency) {
	_target_direction = direction;
	_target_freq = frequency + _turn_freq_bias;
	if (frequency > FREQ_MAX) {
		frequency = FREQ_MAX;
		cerr << "BlackStepper::PANIC: frequency too high, set to maximum frequency " << FREQ_MAX << endl;
	}
	if (frequency < FREQ_MIN) {
		frequency = FREQ_MIN;
		cerr << "BlackStepper::PANIC: frequency too low, set to minimum frequency " << FREQ_MIN << endl;
	}
	// _target_freq = (uint64_t)lround((double)PERIOD_MICRO_TO_FREQ(speed) * (1 + _turn_freq_bias));

	_speedReached = 0;


	if (isLongEnough()) {
		#ifdef STEPPER_DEBUG
		timespec temp;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp);
		cout << "Updating motor motion, last timestamp: ";
		cout << _last_timestamp.tv_sec << "." << _last_timestamp.tv_nsec;
		cout << ", new timestamp: " << temp.tv_sec << "." << temp.tv_nsec << endl;
		cout << "Target direction: " << _target_direction << ", target speed: " << _target_speed << endl;
		#endif
		int _cal_new_freq = static_cast<int>(_current_freq);

		unsigned int real_step = _current_accelration_step;
		if (_current_direction == _target_direction) {
			real_step = std::min( std::abs(_cal_new_freq - static_cast<int>(_target_freq)), real_step);


			// If the target is speed already reached, just return. UNLESS some other program is modifying the gpio
			if (real_step == 0) {
				#ifdef STEPPER_DEBUG
				cout << "Target speed " << _target_speed << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
				#endif
				_speedReached = 1;
				return;
			}

			#ifdef STEPPER_DEBUG
			cout << "real step size set to " << real_step << endl;
			#endif
		}

		#ifdef STEPPER_DEBUG
		cout << "Current direction: " << _current_direction << ", current speed: " << _current_speed << endl;
		#endif

		if (_current_direction == _target_direction && _cal_new_freq < static_cast<int>(_target_freq)) {
			_cal_new_freq += static_cast<int>(real_step);
		} else {
			_cal_new_freq -= static_cast<int>(real_step);
		}

		if (_cal_new_freq < 0) {
			_cal_new_freq = - _cal_new_freq;
			_current_direction = !_current_direction;
		}

		_current_freq = _cal_new_freq;
		// if (_cal_new_freq == 0) {
		// 	_current_speed = PERIOD_MAX;
		// } else {
		// 	_current_speed = FREQ_TO_PERIOD_MICRO(_cal_new_freq);
		// }

		// if (_current_speed > PERIOD_MAX) _current_speed = PERIOD_MAX;
		// if (_current_speed < PERIOD_MIN) _current_speed = PERIOD_MIN;

		#ifdef STEPPER_DEBUG
		cout << "New direction: " << _current_direction << ", new frequency: " << _current_freq << endl;
		cout << "Step size: " << real_step << endl;
		#endif

		setGPIOAndPWM(_current_direction, static_cast<unsigned int>(_cal_new_freq));

		if (_target_direction == _current_direction && _target_freq == _current_freq) {
			#ifdef STEPPER_DEBUG
			cout << "Target frequency " << _target_freq << " and target direction " << _target_direction << " are reached, not doing anything" << endl;
			#endif
			_speedReached = 1;
		}

		// update timestamp
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_last_timestamp);
	}
}

// inline unsigned long BlackStepper::micros() {
//     auto duration = std::chrono::system_clock::now().time_since_epoch();
//     return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
// }

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

inline void BlackStepper::setGPIOAndPWM(bool direction, unsigned int frequency) {
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
}

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
	setGPIOAndPWM(0, 100);
}

void BlackStepper::setAcceleration(unsigned int acceration_step) {
	_current_accelration_step = acceration_step;
}

void BlackStepper::setBias(int bias) {
	// if (bias > BIAS_MAX) {
	// 	bias = BIAS_MAX;
	// }
	// if (bias < - BIAS_MAX) {
	// 	bias = - BIAS_MAX;
	// }
	_turn_freq_bias = bias;
	run();
}

//Getters
bool BlackStepper::getDirection() {
	return _current_direction;
}

unsigned int BlackStepper::getFrequency() {
	return _current_freq;
}

bool BlackStepper::targetSpeedReached() {
	return _speedReached;
}
unsigned int BlackStepper::getAcceleration() {
	return _current_accelration_step;
}

int BlackStepper::getBias() {
	return _turn_freq_bias;
}

//Private functions
void BlackStepper::setMovement(bool direction, unsigned int frequency) {
	bool _real_target_direction;
	unsigned int  _real_target_freq;

	_target_direction = direction;
	_target_freq = frequency;

	_real_target_direction = direction;
	int _tentative_freq = static_cast<int>(frequency) + _turn_freq_bias;
	#ifdef STEPPER_DEBUG
	if (isLongEnough()) {
		cout << "BS::setMovement::Tentative frequency: " << _tentative_freq << " turn bias:  " << _turn_freq_bias << endl;
	}
	#endif
	if (_tentative_freq < FREQ_MIN) {
		_real_target_direction = !_real_target_direction;
		_tentative_freq = 2 * FREQ_MIN - _tentative_freq;
		#ifdef STEPPER_DEBUG
		if (isLongEnough()) {
			cout << "BS::setMovement::Reverting tentative frequency, tentative frequency: " << _tentative_freq << " turn bias:  " << _turn_freq_bias << endl;
		}
		#endif
	}
	_real_target_freq = static_cast<unsigned int>(_tentative_freq);
	#ifdef STEPPER_DEBUG
	if (isLongEnough()) {
		cout << "BS::setMovement::Target frequency from tentative frequency:  " << _real_target_freq << endl;
	}
	#endif
	if (_real_target_freq > FREQ_MAX) {
		_real_target_freq = FREQ_MAX;
		if (isLongEnough()) {
			cerr << "BS::setMovement::BlackStepper::PANIC: frequency too high, set to maximum frequency " << FREQ_MAX << endl;
		}
	}
	if (_real_target_freq < FREQ_MIN) {
		_real_target_freq = FREQ_MIN;
		if (isLongEnough()) {
			cerr << "BS::setMovement::BlackStepper::PANIC: frequency too low, set to minimum frequency " << FREQ_MIN << endl;
		}
	}
	// _real_target_freq = (uint64_t)lround((double)PERIOD_MICRO_TO_FREQ(speed) * (1 + _turn_freq_bias));


	if (_real_target_direction == _current_direction && _real_target_freq == _current_freq) {
		#ifdef STEPPER_DEBUG
		if (isLongEnough()) {
			cout << "BS::setMovement::Target frequency " << _real_target_freq << " and target direction " << _real_target_direction << " are reached" << endl;
		}
		#endif
		_speedReached = true;
		return;
	} else {
		_speedReached = false;
	}


	if (isLongEnough()) {
		#ifdef STEPPER_DEBUG
		timespec temp;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp);
		cout << "BS::setMovement::Updating motor motion, last timestamp: ";
		cout << _last_timestamp.tv_sec << "." << _last_timestamp.tv_nsec;
		cout << ", new timestamp: " << temp.tv_sec << "." << temp.tv_nsec << endl;
		cout << "BS::setMovement::Target direction: " << direction << ", target frequency: " << frequency << ", bias: " << _turn_freq_bias <<  endl;
		cout << "BS::setMovement::Biased direction: " << _real_target_direction << ", biased frequency: " << _real_target_freq << endl;
		#endif
		int _cal_new_freq = static_cast<int>(_current_freq);

		unsigned int real_step = _current_accelration_step;
		if (_current_direction == _real_target_direction) {
			real_step = std::min( std::abs(_cal_new_freq - static_cast<int>(_real_target_freq)), static_cast<int>(real_step));


			// If the target is speed already reached, just return. UNLESS some other program is modifying the gpio
			if (real_step == 0) {
				#ifdef STEPPER_DEBUG
				cout << "BS::setMovement::Target frequency " << _real_target_freq << " and target direction " << _real_target_direction << " are reached" << endl;
				cout << endl;
				#endif
				_speedReached = true;
				return;
			}

			#ifdef STEPPER_DEBUG
			cout << "BS::setMovement::real step size set to " << real_step << endl;
			#endif
		}

		#ifdef STEPPER_DEBUG
		cout << "BS::setMovement::Current direction: " << _current_direction << ", current frequency: " << _current_freq << endl;
		#endif

		if (_current_direction == _real_target_direction && _cal_new_freq < static_cast<int>(_real_target_freq)) {
			_cal_new_freq += static_cast<int>(real_step);
		} else {
			_cal_new_freq -= static_cast<int>(real_step);
		}

		if (_cal_new_freq < FREQ_MIN) {
			#ifdef STEPPER_DEBUG
			cout << "BS::setMovement::Reverting direction to " << !_current_direction << endl;
			#endif
			_cal_new_freq = 2 * FREQ_MIN - _cal_new_freq;
			_current_direction = !_current_direction;
		}

		// _real_target_freq = _cal_new_freq;
		// if (_cal_new_freq == 0) {
		// 	_current_speed = PERIOD_MAX;
		// } else {
		// 	_current_speed = FREQ_TO_PERIOD_MICRO(_cal_new_freq);
		// }

		// if (_current_speed > PERIOD_MAX) _current_speed = PERIOD_MAX;
		// if (_current_speed < PERIOD_MIN) _current_speed = PERIOD_MIN;
		_current_freq = static_cast<unsigned int>(_cal_new_freq);
		#ifdef STEPPER_DEBUG
		cout << "BS::setMovement::New direction: " << _current_direction << ", new frequency: " << _current_freq << endl;
		cout << "BS::setMovement::Step size: " << real_step << endl;
		#endif

		if (_real_target_direction == _current_direction && _real_target_freq == _current_freq) {
			#ifdef STEPPER_DEBUG
			cout << "BS::setMovement::Target frequency " << _real_target_freq << " and target direction " << _real_target_direction << " are reached" << endl;
			#endif
			_speedReached = true;
		}

		setGPIOAndPWM(_current_direction, static_cast<unsigned int>(_current_freq));

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

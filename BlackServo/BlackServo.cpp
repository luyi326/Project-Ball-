#include "BlackServo.h"
#include <unistd.h>

using namespace std;

#define MAX_TIME_NEED_TO_ROTATE_US 1000000

#define SERVO_PERIOD 20
#define DUTY_MIN 2.27f
#define DUTY_MAX 12.14f
#define DUTY_SPAN (DUTY_MAX - DUTY_MIN)

#define TOLERANCE 2

#define BLACK_SERVO_DEBUG

BlackServo::BlackServo(pwmName driver, adcName adcPin):
 _dutycycle(driver),
 _feedback(adcPin),
 angle(90),
 _calibrated(false),
 _adc_pos_low(0.0f),
 _adc_pos_high(0.0f) {
	//initialize pwm and use pwm;
	_dutycycle.setDutyPercent(100.0);
	_dutycycle.setPeriodTime(SERVO_PERIOD * 1000, microsecond);
	_dutycycle.setDutyPercent(50.0);
}

void BlackServo::calibrate() {
	_calibrated = true;
	move_to(0);
	usleep(MAX_TIME_NEED_TO_ROTATE_US);
	_adc_pos_low = _feedback.getConvertedValue(dap3);
	move_to(180);
	usleep(MAX_TIME_NEED_TO_ROTATE_US);
	_adc_pos_high = _feedback.getConvertedValue(dap3);
	#ifdef BLACK_SERVO_DEBUG
	cout << "Servo at 0 degrees reads: " << _adc_pos_low << ", at high degrees reads: " << _adc_pos_high << endl;
	#endif
}

void BlackServo::move_to(int angle) {
	move_to(angle * 1.0f);
	return;
}

void BlackServo::move_to(float angle) {
	if (!_calibrated) {
		cerr << "Servo not calibrated! Need to calibrate servo before use!" << endl;
		//Bypass calibration restrain and move the servo to 90 degrees so the servo won't be damaged
		_calibrated = true;
		move_to(90);
		exit(1);
	}
	if (angle < 0 or angle > 180) {
		return;
	}
	float target_duty = ((angle / 180) * DUTY_SPAN + DUTY_MIN);
	_dutycycle.setDutyPercent(target_duty);
	usleep(200);
#ifdef BLACK_SERVO_DEBUG
		std::cout << "Debug: Target_duty cycle is: " << target_duty << " angle = " << angle << std::endl;
#endif
	return;
}

float BlackServo::current_position() {
	cout << _feedback.getValue() << endl;
	cout << _feedback.getNumericValue() << endl;
	cout << _feedback.getConvertedValue(dap3) << endl;
	return 0.0f;
}

void BlackServo::set_duty_percent(float target_duty) {
	_dutycycle.setDutyPercent(target_duty);
	#ifdef BLACK_SERVO_DEBUG
		cout << "target duty cicle = : " << target_duty << endl;
	#endif
}

BlackServo::~BlackServo() {
	_dutycycle.setDutyPercent(0.0);
	_dutycycle.setPeriodTime(0, BlackLib::microsecond);
	usleep(200);
}

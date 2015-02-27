#include "BlackServo.h"
#include <unistd.h>

#define SERVO_PERIOD 20
#define DUTY_MIN 3
#define DUTY_MAX 14.5
#define DUTY_SPAN (DUTY_MAX - DUTY_MIN)

BlackServo::BlackServo(pwmName driver): _dutycycle(driver) {
	//initialize pwm and use pwm;
	Debug_flag = 0;
	angle = 90;
	_dutycycle.setDutyPercent(100.0);
	_dutycycle.setPeriodTime(SERVO_PERIOD * 1000, microsecond);
	_dutycycle.setDutyPercent(50.0);
}

void BlackServo::move_to(int angle) {
	move_to(angle * 1.0f);
	return;
}

void BlackServo::move_to(float angle) {
	if (angle < 0 or angle > 180) {
		return;
	}
	float target_duty = ((angle / 180) * DUTY_SPAN + DUTY_MIN);
	_dutycycle.setDutyPercent(target_duty);
	usleep(200);
	if (Debug_flag) {
		std::cout << "Debug: Target_duty cycle is: " << target_duty << std::endl;
	}
	return;
}

BlackServo::~BlackServo() {
	_dutycycle.setDutyPercent(0.0);
	_dutycycle.setPeriodTime(0, BlackLib::microsecond);
	usleep(200);
}

void BlackServo::DEBUG_MODE(bool mode) {
	Debug_flag = mode;
}

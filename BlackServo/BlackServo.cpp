#include "BlackServo.h"

#define SERVO_PERIOD 17
#define DUTY_MIN 3
#define DUTY_MAX 14.5
#define DUTY_SPAN (DUTY_MAX - DUTY_MIN)

BlackServo::BlackServo(pwmName driver):_dutycycle(driver){
	//initialize pwm and use pwm;
	Debug_flag=0;
	angle=90;
	_dutycycle.setDutyPercent(100.0);
	_dutycycle.setPeriodTime(SERVO_PERIOD, BlackLib::microsecond);
	_dutycycle.setDutyPercent(50.0);
}

void BlackServo::goto(int angle){
	goto(angle*1.0);
	return;
}

void BlackServo::goto(float angle){
	if (angle<0 or angle > 180){
		return;
	}
	float target_duty=100 - ((angle / 180) * DUTY_SPAN + DUTY_MIN);
	_dutycycle.setDutyPercent(target_duty);
	if (Debug_flag){
		std::cout<<"Debug: Target_duty cycle is: "<<target_duty<<std::endl;
	}
	return;
}

void BlackServo::DEBUG_MODE(bool mode){
	Debug_flag=mode;
}

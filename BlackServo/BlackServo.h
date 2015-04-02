#ifndef _BLACK_SERVO_H_
#define _BLACK_SERVO_H_

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackADC.h"
#include "../BlackLib/BlackGPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace BlackLib;

#define BLACK_SERVO_DEFAULT_TOLERANCE 0.03f

class BlackServo {

public:
	BlackServo(pwmName driver, adcName adcPin);
	~BlackServo();
	void calibrate();
	void move_to(int);
	void move_to(float);
	void set_duty_percent(float);
	float current_position();
	bool target_position_reached();
	void set_tolerance(float);

private:
	BlackPWM _dutycycle;
	BlackADC _feedback;
	float angle;
	bool _calibrated;
	float _adc_pos_low;
	float _adc_pos_high;
	float _adc_span;
	float _tolerance;
};


#endif

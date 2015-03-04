#ifndef _BLACK_SERVO_H_
#define _BLACK_SERVO_H_

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace BlackLib;

class BlackServo {

public:
	BlackServo(pwmName driver);
	~BlackServo();
	void move_to(int);
	void move_to(float);

private:
	BlackPWM _dutycycle;
	float angle;
};


#endif

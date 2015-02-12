#ifndef _BLACK_SERVO_H_
#define _BLACK_SERVO_H_

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackGPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

using namespace BlackLib;

class BlackServo{

public:
	BlackServo(pwmName driver);
	~BlackServo();
	void goto(int);
	void goto(float);
	void DEBUG_MODE(bool);

private:
	BlackPWM _dutycycle;
	void _goto(int);
	bool Debug_flag;
};


#endif
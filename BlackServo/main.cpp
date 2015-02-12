#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackServo.h"

using namespace BlackLib;
using namespace std;

void delay(float ms){
    usleep(lround(ms*1000));
    return;
}

int main (int argc, char* argv[]) {
    BlackServo myServo(EHRPWM1A);
    myServo.DEBUG_MODE(true);
    myServo.goto(0);
    delay(1000);
    myServo.goto(45);
    delay(1000);
    myServo.goto(90);
    delay(1000);
    myServo.goto(120);
    delay(1000);
    myServo.goto(150);
    delay(1000);
    myServo.goto(180);
    delay(1000);
	return 0;
}

#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "./BlackServo.h"

using namespace BlackLib;
using namespace std;

void delay(float ms){
    usleep(lround(ms*1000));
    return;
}

int main (int argc, char* argv[]) {
    BlackServo myServo(EHRPWM2B);
    myServo.DEBUG_MODE(true);
    myServo.move_to(0);
    delay(1000);
    myServo.move_to(45);
    delay(1000);
    myServo.move_to(90);
    delay(1000);
    myServo.move_to(120);
    delay(1000);
    myServo.move_to(150);
    delay(1000);
    myServo.move_to(180);
    delay(1000);
	return 0;
}

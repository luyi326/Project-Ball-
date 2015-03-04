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
    BlackServo myServo(EHRPWM1B);
    while (1) {
        for (int i = 0; i <= 180; i+=2) {
            myServo.move_to(i);
            delay(10);
        }
        for (int i = 180; i >= 0; i-=2) {
            myServo.move_to(i);
            delay(10);
        }
    }
	return 0;
}

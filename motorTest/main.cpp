#include <iostream>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../AccelStepper/AccelStepper.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    AccelStepper stepper(AccelStepper::DRIVER, GPIO_39, GPIO_35);
    int pos = 360;
    int speed = 12000;
    if (arc == 2) {
        speed = atoi(argv[1]);
    }
    stepper.setMaxSpeed(speed);
    stepper.setAcceleration(speed / 3);
    while(1) {
        if (stepper.distanceToGo() == 0) {
            delay(500);
            pos = -pos;
            stepper.moveTo(pos);
        }
        stepper.run();
    }
	return 0;
}
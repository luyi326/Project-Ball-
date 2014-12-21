#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../AccelStepper/AccelStepper.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    AccelStepper stepper(AccelStepper::DRIVER, GPIO_31, GPIO_48);
    int pos = 3600;
    int speed = 12000;
    if (argc == 2) {
        speed = atoi(argv[1]);
    }
    stepper.setMaxSpeed(speed);
    stepper.setAcceleration(speed / 3);
    while(1) {
        if (stepper.distanceToGo() == 0) {
            //usleep(500);
            //pos = -pos;
            pos += 3600;
            stepper.moveTo(pos);
        }
        stepper.run();
    }
	return 0;
}

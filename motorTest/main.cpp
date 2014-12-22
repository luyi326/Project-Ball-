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
    float speed = 12000.0f;
    if (argc == 2) {
        speed = atof(argv[1]);
    }
    cout << "Speed: " << speed << endl;
    stepper.setMaxSpeed(speed);
    stepper.setAcceleration(speed / 3);
    stepper.setMinPulseWidth(20);
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

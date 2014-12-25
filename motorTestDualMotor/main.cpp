#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    DualStepperMotor pair(GPIO_26, EHRPWM2A, GPIO_44, EHRPWM1B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 170;
    if (argc == 2) {
        speed = atoi(argv[1]);
    }
    cout << "running at speed " << speed <<endl;
    pair.moveForward(speed);

    while (!pair.targetSpeedReached()) {
        pair.run();
        usleep(50000);
    }
    sleep(4);

    pair.moveBackward(speed);

    while (!pair.targetSpeedReached()) {
        pair.run();
        usleep(50000);
    }
    sleep(4);

	return 0;
}

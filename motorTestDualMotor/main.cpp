#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    DualStepperMotor pair(GPIO_26, EHRPWM2A, GPIO_44, EHRPWM2B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 170;
    if (argc == 3) {
        speed = atoi(argv[1]);
    }

    pair.moveForward(speed);

    while (!pair.targetSpeedReached()) {
        stepper.run();
    }
    sleep(4);

	return 0;
}

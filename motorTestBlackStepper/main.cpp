#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/BlackStepper.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    BlackStepper stepper(GPIO_44, EHRPWM1B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 400;
    bool direction = 0;
    if (argc == 3) {
    	direction = (bool)atoi(argv[1]);
        speed = atoi(argv[2]);
    }

    stepper.run(direction, speed);
    while (!stepper.targetSpeedReached()) {
    	// cout << "last stepper speed: " << stepper.getSpeed() << endl;
    	stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper.run(!direction, speed);
    while (!stepper.targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper.getSpeed() << endl;
        stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper.run(direction, 200);
    while (!stepper.targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper.getSpeed() << endl;
        stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper.run(direction, 400);
    while (!stepper.targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper.getSpeed() << endl;
        stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper.run(direction, 200);
    while (!stepper.targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper.getSpeed() << endl;
        stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper.run(!direction, 200);
    while (!stepper.targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper.getSpeed() << endl;
        stepper.run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
	return 0;
}

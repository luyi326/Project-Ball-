#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/BlackStepper.h"

using namespace BlackLib;
using namespace std;

BlackStepper* stepper;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        delete stepper;
        exit(0);
    }
}


int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        cout << "Cannot register SIGINT handler" << endl;

    stepper = new BlackStepper(GPIO_15, EHRPWM0B);
    cout << "Setup concluded" << endl;

    int speed = 400;
    if (argc == 2) {
        speed = atoi(argv[1]);
    }

    stepper->adjustSpeed(speed, 0);
    while (!stepper->targetSpeedReached()) {
    	// cout << "last stepper speed: " << stepper->getSpeed() << endl;
        cout << "acceleration is " << stepper->getLinearAcceleration() << " m/s^2" << endl;
        stepper->adjustSpeed(0);
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->adjustSpeed(-speed, 0);
    while (!stepper->targetSpeedReached()) {
        cout << "acceleration is " << stepper->getLinearAcceleration() << " m/s^2" << endl;
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->adjustSpeed(0);
    }
    cout << "Motor in position" << endl;
    sleep(4);
    // stepper->adjustSpeed(200);
    // while (!stepper->targetSpeedReached()) {
    //     // cout << "last stepper speed: " << stepper->getSpeed() << endl;
    //     stepper->adjustSpeed();
    // }
    // cout << "Motor in position" << endl;
    // sleep(4);
    // stepper->adjustSpeed(400);
    // while (!stepper->targetSpeedReached()) {
    //     // cout << "last stepper speed: " << stepper->getSpeed() << endl;
    //     stepper->adjustSpeed();
    // }
    // cout << "Motor in position" << endl;
    // sleep(4);
    // stepper->adjustSpeed(200);
    // while (!stepper->targetSpeedReached()) {
    //     // cout << "last stepper speed: " << stepper->getSpeed() << endl;
    //     stepper->adjustSpeed();
    // }
    // cout << "Motor in position" << endl;
    // sleep(4);
    // stepper->adjustSpeed(-200);
    // while (!stepper->targetSpeedReached()) {
    //     // cout << "last stepper speed: " << stepper->getSpeed() << endl;
    //     stepper->adjustSpeed();
    // }
    // cout << "Motor in position" << endl;
    // sleep(4);

    delete stepper;
	return 0;
}

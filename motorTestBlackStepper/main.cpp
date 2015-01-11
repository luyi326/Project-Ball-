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

    stepper = new BlackStepper(GPIO_44, EHRPWM1B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 400;
    bool direction = 0;
    if (argc == 3) {
    	direction = (bool)atoi(argv[1]);
        speed = atoi(argv[2]);
    }

    stepper->run(direction, speed);
    while (!stepper->targetSpeedReached()) {
    	// cout << "last stepper speed: " << stepper->getSpeed() << endl;
    	stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->run(!direction, speed);
    while (!stepper->targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->run(direction, 200);
    while (!stepper->targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->run(direction, 400);
    while (!stepper->targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->run(direction, 200);
    while (!stepper->targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);
    stepper->run(!direction, 200);
    while (!stepper->targetSpeedReached()) {
        // cout << "last stepper speed: " << stepper->getSpeed() << endl;
        stepper->run();
    }
    cout << "Motor in position" << endl;
    sleep(4);

    delete stepper;
	return 0;
}

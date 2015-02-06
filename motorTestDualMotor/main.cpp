#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"

using namespace BlackLib;
using namespace std;

DualStepperMotor* motorPair;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        delete motorPair;
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        cout << "Cannot register SIGINT handler" << endl;

    motorPair = new DualStepperMotor(GPIO_26, EHRPWM2A, GPIO_44, EHRPWM1B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 170;
    if (argc == 2) {
        speed = atoi(argv[1]);
    }
    cout << "running at speed " << speed <<endl;
    motorPair->moveForward(speed);

    while (!motorPair->targetSpeedReached()) {
        motorPair->run();
        usleep(50000);
    }
    sleep(4);

    motorPair->setBias(128);

    while (!motorPair->targetSpeedReached()) {
        motorPair->run();
        usleep(50000);
    }
    sleep(4);

    motorPair->setBias(-128);

    while (!motorPair->targetSpeedReached()) {
        motorPair->run();
        usleep(50000);
    }
    sleep(4);

    delete motorPair;
	return 0;
}

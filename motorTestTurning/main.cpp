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

    motorPair = new DualStepperMotor(GPIO_15, EHRPWM0A, GPIO_47, EHRPWM2B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 170;
    if (argc >= 2) {
        speed = atoi(argv[1]);
    }
    float bias = 0.0f;
    if (argc >= 3) {
        bias = atof(argv[2]);
    }
    motorPair->setAcceleration(300);
    cout << "running at speed " << speed << " and bias " << bias << endl;
    // exit(0);
    motorPair->moveForward(speed);
    motorPair->setBias(bias);
    motorPair->run();
    // exit(0);
    while (!motorPair->targetSpeedReached()) {
        motorPair->run();
        usleep(50000);
    }
    sleep(4);

    motorPair->moveBackward(speed);

    while (!motorPair->targetSpeedReached()) {
        motorPair->run();
        usleep(50000);
    }
    sleep(4);

    delete motorPair;
	return 0;
}

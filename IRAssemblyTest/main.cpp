#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include "../naughtyException/naughtyException.h"
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"
#include "../PVision/IRRim.h"

using namespace BlackLib;
using namespace std;

DualStepperMotor* motorPair;
IRRim* rim;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        delete motorPair;
        delete rim;
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        cout << "Cannot register SIGINT handler" << endl;

    motorPair = new DualStepperMotor(GPIO_26, EHRPWM2A, GPIO_44, EHRPWM2B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 170;
    // if (argc >= 2) {
    //     speed = atoi(argv[1]);
    // }
    float bias = 0.2;
    // if (argc >= 3) {
    //     bias = atof(argv[2]);
    // }

    motorPair->setAcceleration(100);
    cout << "running at speed " << speed <<endl;
    motorPair->moveForward(speed);
    motorPair->setBias(bias);

    try {
        rim = new IRRim(4, EHRPWM1B, GPIO_48, AIN0);
    } catch (naughty_exception ex) {
        if (ex == naughty_exception_PVisionWriteFail) {
            cerr << "One or more IR sensors are malfunctioning, exiting" << endl;
            exit(1);
        }
    }

    while (1) {
        timespec t1;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

        if (!motorPair->targetSpeedReached()) {
            motorPair->run();
        }
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        // usleep(10000);


        // timespec t2;
        // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);

        // timespec temp;
        // if ((t2.tv_nsec-t1.tv_nsec)<0) {
        //     temp.tv_sec = t2.tv_sec-t1.tv_sec-1;
        //     temp.tv_nsec = 1000000000+t2.tv_nsec-t1.tv_nsec;
        // } else {
        //     temp.tv_sec = t2.tv_sec-t1.tv_sec;
        //     temp.tv_nsec = t2.tv_nsec-t1.tv_nsec;
        // }
        // cout << temp.tv_sec << "." << temp.tv_nsec << endl;
    }

    delete motorPair;
    delete rim;
	return 0;
}

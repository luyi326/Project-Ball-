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

    motorPair = new DualStepperMotor(GPIO_27, EHRPWM2A, GPIO_47, EHRPWM2B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 100000;
    float bias = 0.0f;

    motorPair->setAcceleration(100);
    cout << "running at speed " << speed <<endl;
    motorPair->moveForward(speed);
    motorPair->setBias(bias);

    try {
        rim = new IRRim(4, EHRPWM1B, GPIO_48, AIN0);
    } catch (naughty_exception ex) {
        if (ex == naughty_exception_PVisionInitFail) {
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
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        IR_target target = rim->run();
        bool left_or_right = true;
        uint16_t angle_bias = 0;
        // TODO: Need PID control???
        if (target.distance < -2) target.distance = -target.distance;
        if (target.target_located && target.distance > 0) {
            cout << target << endl;
            if (target.angle >= 0 && target.angle < 180) {
                //turn left?
                left_or_right = true;
                angle_bias = target.angle;
            } else {
                //turn right?
                left_or_right = false;
                angle_bias = 360 - target.angle;
            }
            if (target.distance < 10) {
                cout << "target is too close! abort!" << endl;
                delete motorPair;
                delete rim;
                exit(0);
            } else {
                motorPair->moveForward(uint64_t(500 - (target.distance - 60) * 7.5));
                cout << "Setting speed to " << uint64_t(500 - (target.distance - 60) * 7.5) << endl;
                // motorPair->setBias((~left_or_right) * angle_bias / 180.0f);
                 motorPair->setBias(0.9);
                cout << "Setting bias to " << (~left_or_right) * angle_bias / 180.0f << endl;
            }
        }
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

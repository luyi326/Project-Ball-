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
#include "../PID/PID.h"

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

    motorPair = new DualStepperMotor(GPIO_15, EHRPWM0A, GPIO_47, EHRPWM2B);
    cout << "Setup concluded" << endl;

    uint64_t speed = 100000;
    int bias = 0;

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

    PID pid_turn(0.5, 1.0f / 40, 1.0f / 80, 3, 0);

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
        uint16_t angle_bias = 0;
        // TODO: Need PID control???
        if (target.distance < -2) target.distance = -target.distance;
        if (target.target_located && target.distance > 0) {
            bool left_or_right = true;
            cout << "target aqquired" << endl;
            motorPair->setAcceleration(100);
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
            cout << "angle bias is " << angle_bias << " to the " << (left_or_right?"left":"right") << endl;
            if (target.distance < 10) {
                cout << "target is too close! abort!" << endl;
                delete motorPair;
                delete rim;
                exit(0);
            } else {
                motorPair->moveForward(uint64_t(500 - (target.distance - 60) * 7.5));
                cout << "Setting speed to " << uint64_t(500 - (target.distance - 60) * 7.5) << endl;
                int tmpBias = 0;
                int middle_angle = 0;
                if (left_or_right) {
                    tmpBias = - int(angle_bias) / 0.036f;
                    middle_angle = - int(angle_bias);
                } else {
                    tmpBias = int(angle_bias) / 0.036f;
                    middle_angle = int(angle_bias);
                }
                int middle_error = 0;
                if (middle_angle > 10) {
                    middle_error = middle_angle - 10;
                } else if (middle_angle < -10) {
                    middle_error = middle_angle + 10;
                } else {
                    middle_error = 0;
                }
                int correct = int(lround(pid_turn.kernel(middle_error, middle_angle)));
                bias = tmpBias;
                cout << "correction is " << correct << endl;
                motorPair->setBias(tmpBias);
                 // motorPair->setBias(0.9);
                cout << "Setting bias to " << tmpBias << endl;
            }
        } else {
            cout << "target lost" << endl;
            motorPair->setAcceleration(200);
            motorPair->setBias(0);
            motorPair->stop();


        }
    }

    delete motorPair;
    delete rim;
	return 0;
}

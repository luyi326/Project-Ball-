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

    uint64_t freq = 100;
    int bias = 0;

    motorPair->setAcceleration(100);
    cout << "running at freq " << freq <<endl;
    motorPair->moveForward(freq);
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

    int lost_count = 0;
    while (1) {
        // cout << "main:: ck point 1" << endl;
        timespec t1;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

        if (!motorPair->targetSpeedReached()) {
            motorPair->run();
        }
        // cout << "main:: ck point 2" << endl;
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        // cout << "main:: ck point 3" << endl;
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        // cout << "main:: ck point 4" << endl;
        rim->run();
        rim->run();
        rim->run();
        rim->run();
        IR_target target = rim->run();
        int angle_bias = 0;
        // cout << "main:: ck point 5" << endl;
        // TODO: Need PID control???
        if (target.distance < -2) target.distance = -target.distance;
        if (target.target_located && target.distance > 20) {
            lost_count = 0;
            bool left_or_right = true;
            cout << "target aqquired" << endl;
            motorPair->setAcceleration(100);
            cout << target << endl;
            bool forward = true;
            // cout << "main:: ck point 6" << endl;
            if (target.angle >= 0 && target.angle < 180) {
                //turn left?
                left_or_right = true;
                angle_bias = target.angle;
            } else {
                //turn right?
                left_or_right = false;
                angle_bias = 360 - target.angle;
            }
            // cout << "main:: ck point 7" << endl;
            if (target.angle >= 90 && target.angle < 270) {
                left_or_right = !left_or_right;
                angle_bias = 180 - angle_bias;
                forward = false;
            }
            // cout << "main:: ck point 8" << endl;
            cout << "angle bias is " << angle_bias << " to the " << (left_or_right?"left":"right") << endl;
            if (target.distance < 10) {
                cout << "target is too close! abort!" << endl;
                delete motorPair;
                delete rim;
                exit(0);
            } else {
                cout << "main:: ck point 9: distance = " << target.distance << " new freq = " << freq << endl;
                freq = (target.distance - 20) * 100 + 100;
                if (forward) {
                    motorPair->moveForward(freq);
                    cout << "Setting speed to " << freq << endl;
                } else {
                    motorPair->moveBackward(freq);
                    cout << "Setting speed to " << freq << endl;
                }
                float tmpBias = 0;
                int middle_angle = 0;
                // cout << "main:: ck point 10" << endl;
                if (left_or_right) {
                    middle_angle = - angle_bias;
                    if (forward)
                        tmpBias = angle_bias * 70;
                    else
                        tmpBias = -angle_bias * 70;
                } else {
                    if (forward)
                        tmpBias =  -angle_bias * 70;
                    else
                        tmpBias = angle_bias * 70;
                    middle_angle = angle_bias;
                }
                // cout << "main:: ck point 11" << endl;
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
                // cout << "correction is " << correct << endl;
                motorPair->setBias(tmpBias);
                // cout << "Setting bias to " << tmpBias << endl;
            }
        } else {
            motorPair->setBias(0);
            if (lost_count < 10) {
                lost_count++;
                continue;
            }
            // cout << "target lost" << endl;
            motorPair->setAcceleration(200);
            motorPair->moveForward(100);
            rim->force_seek();
            if (target.target_located) {
                cout << "Target located but distance is too short" << endl;
            }
        }
    }

    delete motorPair;
    delete rim;
	return 0;
}

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

#define BIAS_COEFF 110

DualStepperMotor* motorPair;
IRRim* rim;

void clean_up();

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nMAIN::Received SIGINT" << endl;
        clean_up();
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cerr << "MAIN::Cannot register SIGINT handler" << endl;
    }
    int retry_count = 0;
    for(;;) {
        try {
            motorPair = new DualStepperMotor(GPIO_15, EHRPWM0A, GPIO_47, EHRPWM2B);
            cout << "MAIN::Setup concluded" << endl;

            uint64_t freq = 100;
            int bias = 0;

            motorPair->setAcceleration(100);
            cout << "MAIN::running at freq " << freq <<endl;
            motorPair->moveForward(freq);
            motorPair->setBias(bias);

            try {
                rim = new IRRim(4, EHRPWM1B, GPIO_48, AIN0);
            } catch (naughty_exception ex) {
                if (ex == naughty_exception_PVisionInitFail) {
                    cerr << "MAIN::One or more IR sensors are malfunctioning, exiting" << endl;
                    clean_up();
                    exit(1);
                }
            }

            PID pid_turn(0.5, 1.0f / 40, 1.0f / 80, 3, 0);

            int lost_count = 0;
            for(;;) {
                int angle_bias = 0;
                IR_target target;

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
                target = rim->run();

                if (target.distance < -2) target.distance = -target.distance;
                if (target.target_located && target.distance > 20) {
                    lost_count = 0;
                    bool left_or_right = true;
                    cout << "MAIN::target acquired" << endl;
                    motorPair->setAcceleration(300);
                    cout << target << endl;
                    bool forward = true;
                    if (target.angle >= 0 && target.angle < 180) {
                        //turn left?
                        left_or_right = true;
                        angle_bias = target.angle;
                    } else {
                        //turn right?
                        left_or_right = false;
                        angle_bias = 360 - target.angle;
                    }
                    if (target.angle >= 90 && target.angle < 270) {
                        left_or_right = !left_or_right;
                        angle_bias = 180 - angle_bias;
                        forward = false;
                    }
                    cout << "MAIN::angle bias is " << angle_bias << " to the " << (left_or_right?"left":"right") << endl;
                    if (target.distance < 10) {
                        cout << "MAIN::target is too close! abort!" << endl;
                        delete motorPair;
                        delete rim;
                        exit(0);
                    } else {
                        // freq = (target.distance - 20) * 100 + 100;
                        freq = 5700;
                        cout << "MAIN::distance = " << target.distance << " new freq = " << freq << endl;
                        if (forward) {
                            motorPair->moveForward(freq);
                            cout << "MAIN::Setting speed to " << freq << endl;
                        } else {
                            motorPair->moveBackward(freq);
                            cout << "MAIN::Setting speed to " << freq << endl;
                        }
                        float tmpBias = 0;
                        int middle_angle = 0;
                        if (left_or_right) {
                            middle_angle = - angle_bias;
                            if (forward)
                                tmpBias = angle_bias * BIAS_COEFF;
                            else
                                tmpBias = -angle_bias * BIAS_COEFF;
                        } else {
                            if (forward)
                                tmpBias =  -angle_bias * BIAS_COEFF;
                            else
                                tmpBias = angle_bias * BIAS_COEFF;
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
                        bias = tmpBias;
                        motorPair->setBias(tmpBias);
                        cout << "MAIN::Setting bias to " << tmpBias << endl;
                    }
                } else {
                    motorPair->setBias(0);
                    if (lost_count < 15) {
                        lost_count++;
                        continue;
                    }
                    motorPair->setAcceleration(200);
                    motorPair->moveForward(100);
                    rim->force_seek();
                    if (target.target_located) {
                        cout << "MAIN::Target located but distance is too short" << endl;
                    }
                }
            }
        } catch (...) {
            clean_up();
            retry_count++;
            if (retry_count < 3) {
                continue;
            } else {
                throw;
            }
        }
    }
	return 0;
}

void clean_up() {
    if (motorPair) {
        delete motorPair;
    }
    if (rim) {
        delete rim;
    }
}
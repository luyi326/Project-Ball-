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

timer_t stepperIntID;
timer_t IRRimIntID;


void clean_up();
void handleStepper();
void handleIRRim();

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nMAIN::Received SIGINT" << endl;
        clean_up();
        exit(0);
    }
}

// static void timerHandler( int sig, siginfo_t *si, void *uc ) {
//     timer_t *tidp;

//     tidp = (timer_t*)si->si_value.sival_ptr;

//     if ( *tidp == stepperIntID )
//         handleStepper();
//     else if ( *tidp == IRRimIntID )
//         handleIRRim();
// }

// static int makeTimer( timer_t *timerID, int expireMS, int intervalMS )
// {
//     struct sigevent te;
//     struct itimerspec its;
//     struct sigaction sa;
//     int sigNo = SIGRTMIN;

//     /* Set up signal handler. */
//     sa.sa_flags = SA_SIGINFO;
//     sa.sa_sigaction = timerHandler;
//     sigemptyset(&sa.sa_mask);
//     if (sigaction(sigNo, &sa, NULL) == -1) {
//         perror("sigaction");
//     }

//     /* Set and enable alarm */
//     te.sigev_notify = SIGEV_SIGNAL;
//     te.sigev_signo = sigNo;
//     te.sigev_value.sival_ptr = timerID;
//     timer_create(CLOCK_REALTIME, &te, timerID);

//     its.it_interval.tv_sec = 0;
//     its.it_interval.tv_nsec = intervalMS * 1000000;
//     its.it_value.tv_sec = 0;
//     its.it_value.tv_nsec = expireMS * 1000000;
//     timer_settime(*timerID, 0, &its, NULL);

//     return 1;
// }

string ZeroPadNumber(int num)
{
    stringstream ss;

    // the number is converted to string with the help of stringstream
    ss << num;
    string ret;
    ss >> ret;

    // Append zero chars
    int str_length = ret.length();
    for (int i = 0; i < 9 - str_length; i++)
        ret = "0" + ret;
    return ret;
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cerr << "MAIN::Cannot register SIGINT handler" << endl;
    }


    // int retry_count = 0;
    try {
        motorPair = new DualStepperMotor(GPIO_15, EHRPWM0B, GPIO_27, EHRPWM2A, SPI0_0, GPIO_117, 4.5f, 0.0f, 10.0f);
        cout << "MAIN::Setup concluded" << endl;

        uint64_t freq = 100;
        int bias = 0;

        motorPair->setAcceleration(700);
        // cout << "MAIN::running at freq " << freq <<endl;
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

        // PID pid_turn(0.5, 1.0f / 40, 1.0f / 80, 3, 0);

        // int lost_count = 0;
    } catch (...) {
        clean_up();
        throw;
    }

    // makeTimer(&stepperIntID, 3, 20); //20ms
    // makeTimer(&IRRimIntID, 5, 5); //10ms
    for (;;) {
        // cout << "running " << endl;
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        // rim->run();
        IR_target newTarget = rim->run();
        // cout << newTarget << endl;
        if (newTarget.target_located) {
            // cout << "ir: " << newTarget.angle << endl;
            int angle = newTarget.angle + 100;
            if (angle < 0) {
                angle += 360;
            } else if (angle > 360) {
                angle -= 360;
            }
            if (angle < 15 || angle > 345) {
                motorPair->moveForward(3000);
            } else if (angle <= 180) {
                motorPair->moveLeft(angle);
            } else {
                // motorPair->moveForward(0);
                motorPair->moveRight(angle - 180);
            }
            // cout << "raw: " << angle << endl;
        } else {
            motorPair->moveForward(0);
        }
        motorPair->run();
        // usleep(10);
    }
	return 0;
}

// void handleStepper() {
//     motorPair->run();
// }

// void handleIRRim() {
//     IR_target newTarget = rim->run();
//     if (newTarget.target_located) {
//         cout << newTarget.angle << endl;
//     }
// }


void clean_up() {
    if (motorPair) {
        delete motorPair;
    }
    if (rim) {
        delete rim;
    }
}
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"

using namespace BlackLib;
using namespace std;

DualStepperMotor* motorPair;
// timer_t stepperIntID;
// timer_t IRRimIntID;

// static void timerHandler( int sig, siginfo_t *si, void *uc )
// {
//     timer_t *tidp;

//     tidp = si->si_value.sival_ptr;

//     if ( *tidp == firstTimerID )
//         printf("2ms");
//     else if ( *tidp == secondTimerID )
//         printf("10ms\n");
//     else if ( *tidp == thirdTimerID )
//         printf("100ms\n\n");
// }

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


    unsigned int freq = 0;
    if (argc >= 2) {
        freq = atoi(argv[1]);
    }

    motorPair = new DualStepperMotor(GPIO_15, EHRPWM0B, GPIO_27, EHRPWM2A, SPI0_0, GPIO_117, 4.5f, 0.0f, 10.0f);
    cout << "Setup concluded" << endl;

    motorPair->setAcceleration(400);
    // cout << "running at freq " << freq << " and bias " << bias << endl;
    // exit(0);
    motorPair->moveForward(freq);
    motorPair->run();

    while (1) {
        motorPair->run();
        // usleep(10000);
        // while (!motorPair->targetSpeedReached()) {
        //     usleep(50000);
        // }

        // motorPair->moveBackward(freq);

        // while (!motorPair->targetSpeedReached()) {
        //     motorPair->run();
        //     usleep(50000);
        // }
    }

    delete motorPair;
	return 0;
}

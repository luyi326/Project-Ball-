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
timer_t stepperIntID;
timer_t IRRimIntID;

static void timerHandler( int sig, siginfo_t *si, void *uc )
{
    timer_t *tidp;

    tidp = si->si_value.sival_ptr;

    if ( *tidp == firstTimerID )
        printf("2ms");
    else if ( *tidp == secondTimerID )
        printf("10ms\n");
    else if ( *tidp == thirdTimerID )
        printf("100ms\n\n");
}

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

    makeTimer(&stepperIntID, 2, 2); //2ms
    makeTimer(&IRRimIntID, 10, 10); //10ms

    unsigned int freq = 0;
    if (argc >= 2) {
        freq = atoi(argv[1]);
    }
    unsigned int bias = 0;
    if (argc >= 3) {
        bias = atoi(argv[2]);
    }
    float pGain = 0.08f;
    if (argc >= 4) {
        pGain = atof(argv[3]);
    }
    float iGain = 0.0f;
    if (argc >= 5) {
        iGain = atof(argv[4]);
    }
    float dGain = 0.0f;
    if (argc >= 6) {
        dGain = atof(argv[5]);
    }
    motorPair = new DualStepperMotor(GPIO_15, EHRPWM0B, GPIO_27, EHRPWM2A, SPI0_0, GPIO_117, pGain, iGain, dGain);
    cout << "Setup concluded" << endl;

    motorPair->setAcceleration(300);
    cout << "running at freq " << freq << " and bias " << bias << endl;
    // exit(0);
    motorPair->moveForward(freq);
    motorPair->setBias(bias);
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

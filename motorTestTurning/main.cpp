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

    motorPair = new DualStepperMotor(GPIO_15, EHRPWM0B, GPIO_27, EHRPWM2A, SPI0_0, GPIO_117);
    cout << "Setup concluded" << endl;

    unsigned int freq = 100;
    if (argc >= 2) {
        freq = atoi(argv[1]);
    }
    unsigned int bias = 0;
    if (argc >= 3) {
        bias = atoi(argv[2]);
    }
    motorPair->setAcceleration(300);
    cout << "running at freq " << freq << " and bias " << bias << endl;
    // exit(0);
    motorPair->moveForward(freq);
    motorPair->setBias(bias);
    motorPair->run();

    while (1) {
        motorPair->run();
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

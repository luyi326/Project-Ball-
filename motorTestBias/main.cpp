#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackStepper/BlackStepper.h"

using namespace BlackLib;
using namespace std;

BlackStepper* stepper;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        stepper->stop();
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        cout << "Cannot register SIGINT handler" << endl;

    stepper = new BlackStepper(GPIO_15, EHRPWM0A);
    unsigned freq = 100;
    if (argc >= 2) {
        freq = atoi(argv[1]);
    }
    int bias_step = 1000;
    if (argc >= 3) {
        bias_step = atoi(argv[2]);
    }
    cout << "freq: " << freq << endl;
    stepper->setAcceleration(50);
    stepper->run(1, freq);
    int bias = 0;
    while (bias >= -5500) {
        stepper->setBias(bias);
        cout << "bias is now " << bias << endl;
        while (!stepper->targetSpeedReached()) {
            stepper->run();
        }
        bias -= bias_step;
        sleep(1);
    }
    while (bias <= 5500) {
        stepper->setBias(bias);
        cout << "bias is now " << bias << endl;
        while (!stepper->targetSpeedReached()) {
            stepper->run();
        }
        bias += bias_step;
        sleep(1);
    }
    stepper->stop();
	return 0;
}

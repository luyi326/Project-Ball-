#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/DualStepperMotor.h"
#include "../command_X/controller_halo/controller_halo.h"

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

    motorPair = new DualStepperMotor(GPIO_26, EHRPWM2A, GPIO_44, EHRPWM1B);
    cout << "Setup concluded" << endl;

    Halo_XBee xbee;


    while (1) {
        xbee.refreshValue();
        controller_state state = xbee.getControllerInfo();
        // printf("axis0 = %d, axis3 = %d\n", state.AXIS_0, state.AXIS_3);
        uint8_t direction = state.AXIS_3 < 127;
        // uint8_t leftOrRight = state.AXIS_0 < 127;
        int16_t move  = abs(127 - state.AXIS_3);
        int16_t calculatedPeroid = 10000 - 77*move;
        if (move < 10) calculatedPeroid = 20000;
        int16_t bias = (127 - state.AXIS_0);
        if (abs(bias) < 10) bias = 0;
        bias *= 5;
        // cout << "Moving ";
        // if (direction) cout << "forward ";
        // else cout << "backward ";
        // cout << "speed " << move << " period " << calculatedPeroid << endl;

        // cout << "bias " << bias << endl;
        motorPair->setBias(-bias);
        if (direction) motorPair->moveForward(calculatedPeroid - 200);
        else motorPair->moveBackward(calculatedPeroid);
        usleep(500);
    }

    // uint64_t speed = 170;
    // if (argc == 2) {
    //     speed = atoi(argv[1]);
    // }
    // cout << "running at speed " << speed <<endl;
    // motorPair->moveForward(speed);

    // while (!motorPair->targetSpeedReached()) {
    //     motorPair->run();
    //     usleep(50000);
    // }
    // sleep(4);

    // motorPair->setBias(128);

    // while (!motorPair->targetSpeedReached()) {
    //     motorPair->run();
    //     usleep(50000);
    // }
    // sleep(4);

    // motorPair->setBias(-128);

    // while (!motorPair->targetSpeedReached()) {
    //     motorPair->run();
    //     usleep(50000);
    // }
    // sleep(4);

    delete motorPair;
	return 0;
}

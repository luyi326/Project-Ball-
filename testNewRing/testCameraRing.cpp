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
#include "../PVision/cameraRing.h"
#include "../PID/PID.h"

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nMAIN::Received SIGINT" << endl;
        exit(0);
    }
}


int main (int argc, char* argv[]) {
     int low = 0;
    if (argc >= 2) {
        low = atoi(argv[1]);
    }
     int high = 90;
    if (argc >= 3) {
        high = atoi(argv[2]);
    }
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cerr << "MAIN::Cannot register SIGINT handler" << endl;
    }
    cameraRing ring(EHRPWM1B, GPIO_48, AIN0);
    // ring.scanRange(low, high);
    // cameraRingTest ring0(GPIO_48, channel);
    // cameraRingTest ring1(GPIO_48, 1);
    // cameraRingTest ring2(GPIO_48, 2);
    // cameraRingTest ring3(GPIO_48, 3);
    for (;;) {
        ring.readCamera(cameraRingSensorPairFront);
        ring.readCamera(cameraRingSensorPairBack);
        // ring.run();
    	// ring0.testChannel();
    	// ring1.testChannel();
    	// ring2.testChannel();
    	// ring3.testChannel();
    }
}
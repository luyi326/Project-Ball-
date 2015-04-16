#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackStepper/BlackStepper.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    BlackStepper stepper(GPIO_15, EHRPWM0A);
    unsigned freq = 4000;
    if (argc == 2) {
        freq = atoi(argv[1]);
    }
    cout << "freq: " << freq << endl;
    stepper.run(0, freq);
    stepper.setAcceleration(800);
    while (!stepper.targetSpeedReached()) {
        stepper.run();
    }
    sleep(4);
    stepper.run(1, freq);
    stepper.setAcceleration(800);
    while (!stepper.targetSpeedReached()) {
        stepper.run();
    }
	return 0;
}

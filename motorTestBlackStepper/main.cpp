#include <iostream>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackStepper/BlackStepper.h"

using namespace BlackLib;
using namespace std;

int main (int argc, char* argv[]) {
    BlackStepper stepper(GPIO_31, EHRPWM2A);
    stepper.run(0, 400);
    while (!stepper.targetSpeedReached()) {
    	cout << "last stepper speed: " << stepper.getSpeed() << endl;
    	stepper.run();
    }
	return 0;
}

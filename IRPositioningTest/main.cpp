#include <iostream>
#include <unistd.h>
#include <ctime>
#include "../PVision/IRRim.h"

#define IR_DISTANCE (8.1) //This is in cms

using namespace std;
using namespace BlackLib;

int main (int argc, char* argv[]) {
	IRRim rim(2, EHRPWM1B, GPIO_48);
	cout << std::dec;

	while (1) {
        rim.run();
		usleep(5000);
	}
	return 0;
}
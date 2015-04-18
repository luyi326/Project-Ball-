#include <iostream>
#include <unistd.h>
#include <ctime>
#include "../PVision/IRRim.h"

#define IR_DISTANCE (8.1) //This is in cms

using namespace std;
using namespace BlackLib;

int main (int argc, char* argv[]) {
	IRRim rim(4, EHRPWM1B, GPIO_48, AIN0);
	cout << std::dec;

	while (1) {
        rim.run();
	}
	return 0;
}
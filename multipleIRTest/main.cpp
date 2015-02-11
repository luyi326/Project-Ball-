#include <iostream>
#include <unistd.h>
#include "../PVision/PCA9548A.h"

using namespace std;

int main (int argc, char* argv[]) {
	PCA9548A mux;
	mux.selectChannel(0);
	return 0;
}
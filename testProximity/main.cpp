#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include "../naughtyException/naughtyException.h"
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackLib/BlackADC.h"

#include "../PVision/proximityRing.h"

using namespace BlackLib;
using namespace std;


void clean_up();

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nMAIN::Received SIGINT" << endl;
        clean_up();
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    unsigned int channel = 0;
    if (argc >= 2) {
        channel = atoi(argv[1]);
    }
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cerr << "MAIN::Cannot register SIGINT handler" << endl;
    }
    proximityRing ring;
    for (;;) {
        // ring.checkTarget(channel);
        // ring.checkTarget(1);
        // ring.pollRing();
        // cout << ring.currentTarget() << endl;
    }
	return 0;
}

void clean_up() {
}
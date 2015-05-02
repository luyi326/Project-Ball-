#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../arduinoConnector/arduinoConnector.h"
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackSPI.h"

using namespace std;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nMAIN::Received SIGINT" << endl;
        // clean_up();
        exit(0);
    }
}


int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cerr << "MAIN::Cannot register SIGINT handler" << endl;
    }
    arduinoConnector arduino(SPI0_0);
    for (;;) {
    	cout << "kalman X: " << arduino.angleInfomation(arduinoConnector_KalmanX) << endl;
    	cout << "kalman Y: " << arduino.angleInfomation(arduinoConnector_KalmanY) << endl;
    	cout << "kalman Z: " << arduino.angleInfomation(arduinoConnector_KalmanZ) << endl;
    }
}

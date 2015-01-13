#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../9axis/ADXL345.h"
#include "../9axis/HMC5883L.h"
#include "../9axis/L3G4200D.h"

using namespace std;

ADXL345* accele;
HMC5883L* compass;
L3G4200D* gyro;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        delete accele;
        delete compass;
        delete gyro;
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    // Register sigint
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        cout << "Cannot register SIGINT handler" << endl;
    accele = new ADXL345();
    compass = new HMC5883L();
    gyro = new L3G4200D();

    delete accele;
    delete compass;
    delete gyro;
    return 0;
}
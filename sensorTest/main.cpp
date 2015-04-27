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
    accele->init();
    compass->setScale(2.5);
    gyro->init();
    double accele_x = 0, accele_y = 0, accele_z = 0;
    MagnetometerScaled compass_v;
    usleep(1000);

    while (1) {
        accele_x = accele->readAxis(ADX_X);
        accele_y = accele->readAxis(ADX_Y);
        accele_z = accele->readAxis(ADX_Z);
        compass_v = compass->ReadScaledAxis();
        gyro->sample();
        cout << "Accelerometer values: x = " << accele_x << " y = " << accele_y << " z = " << accele_z << endl;
        cout << "Compass values: x = " << compass_v.XAxis << " y = " << compass_v.YAxis << " z = " << compass_v.ZAxis << endl;
        cout << "Gyroscope values: x = " << gyro->g.x << " y = " << gyro->g.y << " z = " << gyro->g.z << endl;
        // sleep(1);
    }

    delete accele;
    delete compass;
    delete gyro;
    return 0;
}
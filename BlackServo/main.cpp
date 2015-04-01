#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackGPIO.h"
#include "./BlackServo.h"

using namespace BlackLib;
using namespace std;

void delay(float ms){
    usleep(lround(ms*1000));
    return;
}
BlackServo* myServo;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        cout << "\nReceived SIGINT" << endl;
        myServo->move_to(90);
        delay(100);
        delete myServo;
        exit(0);
    }
}


int main (int argc, char* argv[]) {
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        cout << "Cannot register SIGINT handler" << endl;
        return 1;
    }
    myServo = new BlackServo(EHRPWM1B, AIN0);
    myServo->calibrate();
    float precent = 100;
    if (argc >= 2) {
        precent = atoi(argv[1]);
    }
    myServo->move_to(precent);
    // delay(1000);



    while (!myServo->target_position_reached()) {
    }
    delay(1000);
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
    cout << "Target position reached: " << myServo->current_position() << endl;
	return 0;
}

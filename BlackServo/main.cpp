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
    myServo = new BlackServo(EHRPWM1A, AIN0);
    myServo->calibrate();
    float precent = 100;
    if (argc >= 2) {
        precent = atoi(argv[1]);
    }
    myServo->move_to(precent);
    delay(10);
    myServo->current_position();
    myServo->current_position();
    myServo->target_position_reached();



    // while (1) {
    //     for (int i = 0; i <= 180; i+=1) {
    //         myServo.move_to(i);
    //         myServo.current_position();
    //         delay(1);
    //     }
    //     for (int i = 180; i >= 0; i-=1) {
    //         myServo.move_to(i);
    //         myServo.current_position();
    //         delay(1);
    //     }
    // }
	return 0;
}

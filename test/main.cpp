#include <iostream>
#include "../BlackLib/BlackLib.h"
// #include <BlackPWM.h>

#include "Timing.h"

using namespace BlackLib;
void exampleAndTiming_PWM();
void test5Mhz(int freq);
void test5Khz(int freq);

int main (int argc, char *argv[]) {
    int freq=atoi(argv[1]);
	// exampleAndTiming_PWM();
    test5Mhz(freq);
    //test5Khz(freq);
	return 0;
}

void test5Khz(int freq) {
    BlackLib::BlackPWM  pwmLed(BlackLib::EHRPWM1A);
    pwmLed.setDutyPercent(100.0);
    pwmLed.setPeriodTime(5000, BlackLib::nanosecond);
    pwmLed.setDutyPercent(60.0);
    sleep(1);
}


void test5Mhz(int freq) {
    BlackLib::BlackPWM  pwmLed(BlackLib::EHRPWM1A);
    pwmLed.setDutyPercent(100.0);
    
    pwmLed.setPeriodTime(ceil(1000/freq), BlackLib::nanosecond);
    pwmLed.setDutyPercent(50.0);
    std::cout<<"period time is: "<<pwmLed.getNumericPeriodValue()<<std::endl;
    sleep(1);
}

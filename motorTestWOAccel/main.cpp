#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackPWM.h"
using namespace BlackLib;
using namespace std;

void test1Khz(int period);

int main (int argc, char *argv[]) {
    int period = 1000;
    if (argc == 2) {
        period = atoi(argv[1]);
    }
    test1Khz(period);
	return 0;
}

void test1Khz(int period) {
    BlackLib::BlackPWM  pwmLed(BlackLib::EHRPWM2A);
    cout << "1" << endl;
    pwmLed.setDutyPercent(100.0);
    cout << "2" << endl;
    pwmLed.setPeriodTime(period, BlackLib::microsecond);
    cout << "3" << endl;
    pwmLed.setDutyPercent(60.0);
    cout << "4" << endl;
    sleep(1);
}

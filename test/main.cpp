#include <iostream>
#include "../BlackLib/v2_0/BlackLib.h"
// #include <BlackPWM.h>

#include "Timing.h"

using namespace BlackLib;
void exampleAndTiming_PWM();
void test5Mhz();
void test5Khz();

int main () {
	// exampleAndTiming_PWM();
    //test5Mhz();
    test5Khz();
    while(1) {}
	return 0;
}

void test5Khz() {
    BlackLib::BlackPWM  pwmLed(BlackLib::EHRPWM2A);
    pwmLed.setDutyPercent(50.0);
    pwmLed.setPeriodTime(200, BlackLib::microsecond);
}

void test5Mhz() {
    BlackLib::BlackPWM  pwmLed(BlackLib::EHRPWM2A);
    pwmLed.setDutyPercent(60.0);
    pwmLed.setPeriodTime(200, BlackLib::microsecond);
}

void exampleAndTiming_PWM()
{
    Timing pwmTiming;

    float percent = 5.0;
    std::string currentDuty;
    std::string currentPeriod;
    std::string currentPercentValue;
    std::string currentPolarity;
    std::string currentRun;

    int64_t currentDuty_numeric;
    int64_t currentPeriod_numeric;
    float currentPercentValue_numeric;





    pwmTiming.startMeasure("1. Constructor");
    BlackLib::BlackPWM    pwmLed(BlackLib::EHRPWM2A);
    pwmTiming.endMeasure("1. Constructor");


    // if new period value is less than the current duty value, the new period value setting
    // operation couldn't execute. So firstly duty value is set to zero for safe steps.
    pwmLed.setDutyPercent(0.0);
    pwmLed.setPeriodTime(100000);
    sleep(1);




    pwmTiming.startMeasure("2. BlackPWM::setSpaceRatioTime()");
    pwmLed.setSpaceRatioTime(67, BlackLib::microsecond);
    pwmTiming.endMeasure("2. BlackPWM::setSpaceRatioTime()");

    std::cout << "DUTY after setting space time: \t\t" << pwmLed.getDutyValue() << std::endl;





    pwmTiming.startMeasure("3. BlackPWM::setPeriodTime()");
    pwmLed.setPeriodTime(300000000, BlackLib::picosecond);
    pwmTiming.endMeasure("3. BlackPWM::setPeriodTime()");

    std::cout << "PERIOD after setting period time: \t" << pwmLed.getPeriodValue() << std::endl;





    pwmTiming.startMeasure("4. BlackPWM::setLoadRatioTime()");
    pwmLed.setLoadRatioTime(15000, BlackLib::nanosecond);
    pwmTiming.endMeasure("4. BlackPWM::setLoadRatioTime()");

    std::cout << "DUTY after setting load time: \t\t" << pwmLed.getDutyValue() << std::endl;





    pwmTiming.startMeasure("5. BlackPWM::setDutyPercent()");
    pwmLed.setDutyPercent(11.75);
    pwmTiming.endMeasure("5. BlackPWM::setDutyPercent()");

    std::cout << "DUTY after setting percent: \t\t" << pwmLed.getDutyValue() << std::endl;





    pwmTiming.startMeasure("6_1. BlackPWM::getDutyValue()");
    currentDuty = pwmLed.getDutyValue();
    pwmTiming.endMeasure("6_1. BlackPWM::getDutyValue()");

    pwmTiming.startMeasure("6_2. BlackPWM::getNumericDutyValue()");
    currentDuty_numeric = pwmLed.getNumericDutyValue();
    pwmTiming.endMeasure("6_2. BlackPWM::getNumericDutyValue()");



    pwmTiming.startMeasure("7_1. BlackPWM::getPeriodValue()");
    currentPeriod = pwmLed.getPeriodValue();
    pwmTiming.endMeasure("7_1. BlackPWM::getPeriodValue()");

    pwmTiming.startMeasure("7_2. BlackPWM::getNumericPeriodValue()");
    currentPeriod_numeric = pwmLed.getNumericPeriodValue();
    pwmTiming.endMeasure("7_2. BlackPWM::getNumericPeriodValue()");




    pwmTiming.startMeasure("8_1. BlackPWM::getValue()");
    currentPercentValue = pwmLed.getValue();
    pwmTiming.endMeasure("8_1. BlackPWM::getValue()");

    pwmTiming.startMeasure("8_2. BlackPWM::getNumericValue()");
    currentPercentValue_numeric = pwmLed.getNumericValue();
    pwmTiming.endMeasure("8_2. BlackPWM::getNumericValue()");




    pwmTiming.startMeasure("9_1. BlackPWM::getPolarityValue()");
    currentPolarity = pwmLed.getPolarityValue();
    pwmTiming.endMeasure("9_1. BlackPWM::getPolarityValue()");

    pwmTiming.startMeasure("9_2. BlackPWM::getRunValue()");
    currentRun = pwmLed.getRunValue();
    pwmTiming.endMeasure("9_2. BlackPWM::getRunValue()");












    std::cout << "Duty_str: \t"     << currentDuty << std::endl;
    std::cout << "Duty_num: \t"     << currentDuty_numeric << std::endl;
    std::cout << "Period_str: \t"   << currentPeriod << std::endl;
    std::cout << "Period_num: \t"   << currentPeriod_numeric << std::endl;
    std::cout << "Percent_str: \t"  << currentPercentValue << std::endl;
    std::cout << "Percent_num: \t"  << currentPercentValue_numeric << std::endl;
    std::cout << "Polarity_str: \t" << currentPolarity << std::endl;
    std::cout << "Run_str: \t"      << currentRun << std::endl;



    while( ! pwmLed.fail(BlackLib::BlackPWM::outOfRangeErr) )
    {
        pwmLed.setDutyPercent(percent);

        std::cout << std::endl << "New percent value: " << percent << std::endl;

        percent += 25.0;
        usleep(500000);
    }

    std::cout << std::endl << "Percent value is out of range." << std::endl;


    std::cout << pwmTiming.getAllMeasureTable();
    pwmTiming.saveToFile("BlackPWM_timing_results.out");

}

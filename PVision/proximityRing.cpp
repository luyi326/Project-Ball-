#include <iostream>
#include <unistd.h>
#include "proximityRing.h"

using namespace std;

#define IN_ADC(i) (i%2)
#define REAL_CHANNEL(i) (i/2)

#define SAW_THREASHOLD (1200)

proximityRing::proximityRing() :
lowADC(ADS1115_ADDRESS_LOW),
highADC(ADS1115_ADDRESS_HIGH),
current_target() {
	current_target.degree_low = 0;
	current_target.degree_high = 0;
	current_target.valid = false;
}

void proximityRing::pollRing() {
	uint16_t resultList[8];
	uint8_t sawCount = 0;
	for (uint8_t i = 0; i < 4; i++) {
		resultList[i] = lowADC.readADC_SingleEnded(i);
		if (resultList[i] < SAW_THREASHOLD) {
			sawCount++;
		}
		// cout << "ADC low " << int(i) << ": " << lowADC.readADC_SingleEnded(i) << endl;
	}
	for (uint8_t i = 0; i < 4; i++) {
		resultList[i + 4] = highADC.readADC_SingleEnded(i);
		if (resultList[i] < SAW_THREASHOLD) {
			sawCount++;
		}
		// cout << "ADC high " << int(i) << ": " << highADC.readADC_SingleEnded(i) << endl;
	}

	cout << highADC.readADC_SingleEnded(3) << endl;
}

proximity_target proximityRing::checkTarget(uint8_t sensor_index) {
	proximity_target new_target;
	new_target.valid = false;
	if (!IN_ADC(sensor_index)) {
		// lowADC.readADC_SingleEnded(REAL_CHANNEL(sensor_index));
	} else {
		highADC.readADC_SingleEnded(REAL_CHANNEL(sensor_index));
	}
	return current_target;
}


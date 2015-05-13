#include <iostream>
#include <unistd.h>
#include "proximityRing.h"

using namespace std;

#define IN_ADC(i) (i%2)
#define REAL_CHANNEL(i) (i/2)

proximityRing::proximityRing() :
lowADC(ADS1115_ADDRESS_LOW),
highADC(ADS1115_ADDRESS_HIGH),
current_target() {
	current_target.degree = 0;
	current_target.distance = 0;
	current_target.valid = false;
}

void proximityRing::pollRing() {
	for (uint8_t i = 0; i < 4; i++) {
		cout << "ADC low " << int(i) << ": " << lowADC.readADC_SingleEnded(i) << endl;
	}
	for (uint8_t i = 0; i < 4; i++) {
		cout << "ADC high " << int(i) << ": " << highADC.readADC_SingleEnded(i) << endl;
	}
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


#include <iostream>
#include <vector>
#include <unistd.h>
#include "proximityRing.h"

using namespace std;

#define IN_ADC(i) (i%2)
#define REAL_CHANNEL(i) (i/2)

#define CALIBRATION_COUNT (100)
#define SAW_THREASHOLD_PRECENT (0.95f)

#define THREASHOLD (1300)


ostream& operator<<(ostream& os, const proximity_target& t) {
	if (t.valid) {
		os << "(l: " << t.degree_low << ", h: " << t.degree_high << ")";
	} else {
		os << "(INVALID)";
	}
	return os;
}

proximityRing::proximityRing() :
lowADC(ADS1115_ADDRESS_LOW),
highADC(ADS1115_ADDRESS_HIGH),
current_target(),
threadshold{0, 0, 0, 0, 0, 0, 0, 0}
{
	current_target.degree_low = 0;
	current_target.degree_high = 0;
	current_target.valid = false;
	// calibrate();
	// threadshold[0] = 1300;
	// threadshold[1] = 1300;
	// threadshold[2] = 1200;
	// threadshold[3] = 1250;
	// threadshold[4] = 1300;
	// threadshold[5] = 1300;
	// threadshold[6] = 1300;
	// threadshold[7] = 1300;
}

void proximityRing::pollRing() {
	uint16_t resultList[8];
	vector<uint8_t> sawList;
	for (uint8_t i = 0; i < 4; i++) {
		resultList[i] = lowADC.readADC_SingleEnded(i);
		if (resultList[i] < THREASHOLD) {
			sawList.push_back(i);
		}
		cout << "ADC low " << int(i) << ": " << lowADC.readADC_SingleEnded(i) << endl;
	}
	for (uint8_t i = 0; i < 4; i++) {
		resultList[i + 4] = highADC.readADC_SingleEnded(i);
		if (resultList[i] < THREASHOLD) {
			sawList.push_back(i);
		}
		cout << "ADC high " << int(i) << ": " << highADC.readADC_SingleEnded(i) << endl;
	}
	if (sawList.size() > 4) {
		//something is not right
		invalidateCurrentResult();
	} else if (sawList.size() > 1) {
		//More than one sensor saw target
		//
		//Check inconsistency
		for (vector<uint8_t>::iterator it = sawList.begin() + 1; it != sawList.end(); it++) {
			//Condition for proximity sensor to be next to each other is that their index differs by 1 OR they are the first sensor ant the last sensor
			if (*it - *(it - 1) != 1 && (*it == 0 && *(it - 1) == 7)) {
				//gap! Oh shit!
				invalidateCurrentResult();
				return;
			}
		}
		current_target.degree_low = 23 + sawList[0] * 45;
		current_target.degree_high = 23 +  sawList[sawList.size() - 1] * 45;
		current_target.valid = true;

	} else if (sawList.size() != 0) {
		//one sensor saw target, estimate a bound around the center angle
		current_target.degree_low = sawList[0] * 45;
		current_target.degree_high = (sawList[0] + 1) * 45;
		current_target.valid = true;

	} else {
		//didn't see shit
		invalidateCurrentResult();
	}
	// cout << highADC.readADC_SingleEnded(2) << endl;
}

void proximityRing::testChannel(uint8_t index) {
	if (index < 4) {
		cout << lowADC.readADC_ContineousSampleAvg(index, 40) << endl;
	} else {
		cout << highADC.readADC_ContineousSampleAvg(index - 4, 40) << endl;
	}
}

proximity_target proximityRing::currentTarget() {
	return current_target;
}

proximity_target proximityRing::checkTarget(uint8_t sensor_index) {
	proximity_target new_target;
	new_target.valid = false;
	if (!IN_ADC(sensor_index)) {
		cout << lowADC.readADC_SingleEnded(REAL_CHANNEL(sensor_index)) << endl;
	} else {
		cout << highADC.readADC_SingleEnded(REAL_CHANNEL(sensor_index)) << endl;
	}
	return current_target;
}

void proximityRing::calibrate() {
	for (uint8_t i = 0; i < 8; i++) {
		threadshold[i] = 0;
		int tmp = 0;
		if (i < 4) {
			for (uint16_t j = 0; j < CALIBRATION_COUNT; j++) {
				tmp += lowADC.readADC_SingleEnded(i);
				// cout << "single result: " << lowADC.readADC_SingleEnded(i) << endl;
			}
			threadshold[i] = int(float(tmp) / CALIBRATION_COUNT - 80);
		} else {
			for (uint16_t j = 0; j < CALIBRATION_COUNT; j++) {
				tmp += highADC.readADC_SingleEnded(i - 4);
			}
			threadshold[i] = int(float(tmp) / CALIBRATION_COUNT - 80);
		}
		cout << "threadshold " << int(i) << ": " << int(threadshold[i]) << endl;
	}
}

void proximityRing::invalidateCurrentResult() {
	current_target.degree_low = 0;
	current_target.degree_high = 0;
	current_target.valid = false;
}


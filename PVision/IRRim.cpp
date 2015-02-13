#include "IRRim.h"
#include <stdlib.h>

#define PV_N(n) (1 << (n+2))

//Constructor and destructor
IRRim::IRRim(uint8_t num_of_sensors) {
	if (num_of_sensors > 6) {
		cerr << "Number of sensors is " << num_of_sensors << ", maximum is 6" << endl;
		exit(1);
	}


	//Initialize all IRs and check status
	sensors = new PVision[num_of_sensors];
	sensor_count = num_of_sensors;
	for (int i = 0; i < sensor_count; i++) {
		mux.selectChannel(PV_N(i + 1));
		if (!sensors[i].init()) {
			cerr << "Sensor No. " << i + 1 << " not initialized correctly" << endl;
		}
	}

}

IRRim::~IRRim() {
	mux.reset();
	delete sensors;
}

/**
 * [IRRim::reset description]
 */
void IRRim::reset() {
	mux.reset();
}

void IRRim::poll() {

}

void IRRim::nextSensor() {
}

void IRRim::select(uint8_t num) {
}
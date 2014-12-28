#include "IRRim.h"

//Constructor and destructor
IRRim::IRRim(gpioName mux[NUM_SEL], string calibrationFiles[NUM_IR]) {
	//Initialize mux and point to IR No.0
	for (int i = 0; i < NUM_SEL; i++) {
		_muxs.push_back(BlackGPIO(mux[i], output, SecureMode));
		_muxs[i].setValue(low);
	}
	//Initialize all IRs and check status
	for (int i = 0; i < NUM_IR; i++) {
		_sensors.push_back(PVision());
		_sensors[i].init();
		if (!_sensors[i].isSensorReady()) {
			cout << "IR No. " << i << " is not ready, may need to do a reset" << endl;
		}
		nextSensor();
	}
}

IRRim::~IRRim() {
	_muxs.clear();
	_sensors.clear();
}

/**
 * [IRRim::reset description]
 */
void IRRim::reset() {
	for (vector<PVision>::iterator it = _sensors.begin(); it != _sensors.end(); ++it) {
		(*it).reset();
		nextSensor();
	}
}

void IRRim::poll() {

}

void IRRim::nextSensor() {
	uint8_t current = 0;
	for (vector<BlackGPIO>::reverse_iterator it = _muxs.rbegin(); it != _muxs.rend(); ++it) {
		current |= (*it).getNumericValue();
		current <<= 1;
	}
	current++;

	//Next sensor to the last sensor is sensor No.0
	if (current > NUM_IR - 1) {
		current = 0;
	}
	select(current);
}

void IRRim::select(uint8_t num) {
	uint8_t mask = 0x01;
	for (vector<BlackGPIO>::iterator it = _muxs.begin(); it != _muxs.end(); ++it) {
		(*it).setValue((digitalValue)(num & mask));
		num >>= 1;
	}
}
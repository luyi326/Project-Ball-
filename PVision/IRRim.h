#ifndef IRRim_h
#define IRRim_h

#include <iostream>
#include <vector>
#include "PVision.h"
#include "../BlackLib/BlackCore.h"
#include "../BlackLib/BlackGPIO.h"

#define NUM_IR 8
#define NUM_SEL 3

using namespace std;
using namespace BlackLib;

class IRRim {
public:
	/**
	 *
	 */
	IRRim(gpioName mux[NUM_SEL], string calibrationFiles[NUM_IR]);

	/**
	 *
	 */
	~IRRim();

	/**
	 * [reset description]
	 */
	void reset();

	/**
	 * [poll description]
	 */
	void poll();
private:
	vector<BlackGPIO> _muxs;
	vector<PVision> _sensors;

	void nextSensor();
	void select(uint8_t num);
};

#endif
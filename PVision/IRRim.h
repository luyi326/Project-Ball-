#ifndef IRRim_h
#define IRRim_h

#include <iostream>
#include <vector>
#include "PVision.h"
#include "PCA9548A.h"
#include "../BlackLib/BlackCore.h"
#include "../BlackLib/BlackGPIO.h"


using namespace std;
using namespace BlackLib;

class IRRim {
public:
	/**
	 *	Constructor of IRRim
	 *	mux is an array of mux GPIO bits from MSB to LSB
	 */
	IRRim(uint8_t num_of_sensors);

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
	PCA9548A mux;
	PVision* sensors;
	uint8_t sensor_count;

	void nextSensor();
	void select(uint8_t num);
};

#endif
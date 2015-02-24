#ifndef IRRim_h
#define IRRim_h

#include <iostream>
#include <vector>
#include <ctime>
#include "PVision.h"
#include "PCA9548A.h"
#include "../BlackLib/BlackCore.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackServo/BlackServo.h"

using namespace std;
using namespace BlackLib;

enum IRRimState {
	IRRimState_Seeking,
	IRRimState_targetFound
};

class IRRim {
public:
	/**
	 *	Constructor of IRRim
	 *	mux is an array of mux GPIO bits from MSB to LSB
	 */
	IRRim(uint8_t num_of_sensors, pwmName servoPin, gpioName muxResetPin);

	/**
	 *
	 */
	~IRRim();

	/**
	 * [reset description]
	 */
	void reset();

	/**
	 * @brief Update timestamp and do stuff
	 * @details [long description]
	 */
	void run();

	bool read_IR(uint8_t index);

private:
	PCA9548A mux;
	PVision* sensors;
	BlackServo servo;
	uint8_t sensor_count;
	IRRimState state;
	timespec current_time;

	uint8_t servo_current_position;
	bool is_seeking;
	bool seeking_is_upwared;

	void nextSensor();
	void select(uint8_t num);
	void seek();
	void follow();

	inline timespec time_diff(timespec t1, timespec t2);
};

#endif
#ifndef IRRim_h
#define IRRim_h

#include <iostream>
#include <vector>
#include <ctime>
#include "PVision.h"
#include "PCA9548A.h"
#include "vec.h"
#include "../BlackLib/BlackCore.h"
#include "../BlackLib/BlackGPIO.h"
#include "../BlackLib/BlackPWM.h"
#include "../BlackServo/BlackServo.h"
#include "../naughtyException/naughtyException.h"

//TODO: Resolve write fail errors and deallocation delay

using namespace std;
using namespace BlackLib;

enum IRRimState {
	IRRimState_seeking,
	IRRimState_targetFound,
	IRRimState_reversing
};

enum IRSensorPair {
	IRSensorPairFront,
	IRSensorPairBack
};

enum IRReadResult {
	IRReadResultLost,
	IRReadResultBlobOnLeft,
	IRReadResultBlobOnRight,
	IRReadResultMiddle
};

class IRRim {
public:
	/**
	 *	Constructor of IRRim
	 *	mux is an array of mux GPIO bits from MSB to LSB
	 */
	IRRim(uint8_t num_of_sensors, pwmName servoPin, gpioName muxResetPin, adcName feedbackPin);

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

	IRReadResult read_IR(IRSensorPair pair);

private:
	PCA9548A mux;
	PVision* sensors;
	BlackServo servo;
	uint8_t sensor_count;
	IRRimState state;
	timespec current_time;
	int current_iteration;
	int current_lower_bound;
	int current_upper_bound;

	uint8_t servo_current_position;
	bool is_seeking;
	bool seeking_is_upwared;

	const vec o_left;
	const vec o_right;
	const vec o_left_m_right;

	void nextSensor();
	void select(uint8_t num);
	void seek();
	void follow();
	void reverse();

	inline void validateBlob(uint8_t index_left, uint8_t index_right);

	inline timespec time_diff(timespec t1, timespec t2);

	inline vec calculate_target_coordinate(int left_x, int left_y, int right_x, int right_y);
	inline vec get_directional_vec(int x, int y);
	inline void calculate_intersection_point(vec directional_left, vec directional_right, float& z_left, float& z_right);
};

#endif

#ifndef IRRim_h
#define IRRim_h

#include <iostream>
#include <vector>
#include <ctime>
#include "PVision.h"
#include "PCA9548A.h"
#include "IR_target.h"
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
	IRSensorPairBack,
	IRSensorPairInvalid
};

enum IRReadResult {
	IRReadResultLost,
	IRReadResultBlobOnLeft,
	IRReadResultBlobOnRight,
	IRReadResultMiddle
};

typedef struct {
	float dState; // Last position input
	float iState; // Integrator state
	float iMax, iMin; // Maximum and minimum allowable integrator stat
	float iGain; // integral gain
	float pGain; // proportional gain
	float dGain; // derivative gain

} PID_IRRim;

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param num_of_sensors [description]
 * @param servoPin [description]
 * @param muxResetPin [description]
 * @param feedbackPin [description]
 * @return [description]
 */
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
	IR_target run();

	void force_seek();

	IRReadResult read_IR(IRSensorPair pair);
	IRReadResult read_IR(IRSensorPair pair, Blob* left_avg, Blob* right_avg);

private:
	PCA9548A mux;
	PVision* sensors;
	BlackServo servo;
	uint8_t sensor_count;
	IRRimState state;
	IRSensorPair current_active_pair;
	timespec target_last_seen;
	int current_iteration;
	int current_lower_bound;
	int current_upper_bound;

	uint8_t servo_current_position;
	IRRimState targeting_state;
	bool seeking_is_upwared;
	IR_target dummy_target;
	IR_target last_target;

	const vec o_left;
	const vec o_right;
	const vec o_left_m_right;

	void nextSensor();
	void select(uint8_t num);
	void seek();
	IR_target follow(IRSensorPair);
	/**
	 * @brief 0 for goto 0, 1 for goto 180
	 * @details [long description]
	 *
	 * @param direction [description]
	 */
	void reverse();

/**
 * @brief This function will return
 * @details [long description]
 *
 * @param pair [description]
 * @param pv1 [description]
 * @param pv2 [description]
 * @return return True when the reading is OK, False otherwise
 */
	inline bool read_IR_read_sensor(IRSensorPair pair, PVsion* pv1, PVision* pv2);

	inline void validateBlob(uint8_t index_left, uint8_t index_right);

	// inline timespec time_diff(timespec t1, timespec t2);

	/**
	 * @brief [brief description]
	 * @details the target coordinate might actually correspond to x->x and y->z since y axis is the axis
	 * looking forward and z is pointing upward
	 *
	 * @param left_x [description]
	 * @param left_y [description]
	 * @param right_x [description]
	 * @param right_y [description]
	 * @return [description]
	 */
	inline vec calculate_target_coordinate(int left_x, int left_y, int right_x, int right_y);
	inline vec get_directional_vec(int x, int y);
	inline void calculate_intersection_point(vec directional_left, vec directional_right, float& z_left, float& z_right);

	inline void handle_sensor_exception(naughty_exception e, uint8_t sensor_index);
};

#endif

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
 * @brief Target information
 *
 * @param target_located 	Indicate if this is a package containing valid target information, will be false
 *                       	if no target is seen within vision
 * @param angle 			angle of target relatvie to the ball, 0-360
 * @param distance 			distance of the target from the ball in centimeters, if negative, means a distance
 *                    		is not yet available (when following is still calculating)
 */
typedef struct {
	bool target_located;
	int angle; //clockwise, up front is 0 degrees
	float distance; // distance in cm
} IR_target;
ostream& operator<<(ostream& os, const IR_target& t);

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
	IRRimState seeking_state;
	IRSensorPair current_active_pair;
	timespec target_last_seen;
	int current_iteration;
	int current_lower_bound;
	int current_upper_bound;

	float servo_current_position;
	// bool is_seeking;
	bool seeking_is_upwared;
	IR_target dummy_target;
	IR_target last_target;
	bool should_reverse;

	const vec o_left;
	const vec o_right;
	const vec o_left_m_right;

	void nextSensor();
	void select(uint8_t num);
	void seek();
	IR_target follow(IRSensorPair);
	void reverse();
	void inspect_sensors();

	inline void validateBlob(uint8_t index_left, uint8_t index_right);

	// inline timespec time_diff(timespec t1, timespec t2);

	inline vec calculate_target_coordinate(int left_x, int left_y, int right_x, int right_y);
	inline vec get_directional_vec(int x, int y);
	inline void calculate_intersection_point(vec directional_left, vec directional_right, float& z_left, float& z_right);
};

#endif

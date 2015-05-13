#ifndef IR_TARGET_H
#define IR_TARGET_H

#include <stdint.h>
#include <iostream>

using namespace std;

typedef enum {
	IR_target_source_camera,
	IR_target_source_proximity

} IR_target_source;

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
	bool distance_available;
	uint16_t angle; //clockwise, up front is 0 degrees
	float distance_two_cam; // distance in cm
	float distance_one_cam;
	IR_target_source source;
} IR_target;

ostream& operator<<(ostream& os, const IR_target& t);

IR_target IR_target_running_avg(IR_target new_target);

#endif

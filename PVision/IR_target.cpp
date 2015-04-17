#include "IR_target.h"

ostream& operator<<(ostream& os, const IR_target& t) {
	if (t.target_located) {
		os << "(" << t.angle << " degrees, ";
		if (t.distance_available) {
			os << t.distance << " centimeters)";
		} else {
			os << "-- centimeters)";
		}
	} else {
		os << "(INVALID)";
	}
	return os;
}

IR_target IR_target_running_avg(IR_target new_target) {
	static IR_target* history = new IR_target[3];
}

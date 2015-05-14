#ifndef PROXIMITY_RING
#define PROXIMITY_RING

#include <stdint.h>
#include <stdbool.h>
#include "../ADS1X15/ADS1X15.h"

typedef struct {
	int degree_low;
	int degree_high;
	// int distance;
	bool valid;
} proximity_target;

class proximityRing {
private:
	Adafruit_ADS1015 lowADC;
	Adafruit_ADS1015 highADC;
	proximity_target current_target;
public:
	proximityRing();
	void pollRing();
	proximity_target checkTarget(uint8_t sensor_index);
};

#endif

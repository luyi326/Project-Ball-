#ifndef PROXIMITY_RING
#define PROXIMITY_RING

#include <stdint.h>
#include <stdbool.h>
#include "../ADS1X15/ADS1X15.h"

typedef struct {
	int degree_low;
	int degree_high;
	bool valid;
} proximity_target;

ostream& operator<<(ostream& os, const proximity_target& t);

class proximityRing {
private:
	Adafruit_ADS1015 lowADC;
	Adafruit_ADS1015 highADC;
	proximity_target current_target;
	uint16_t threadshold[8];

	void invalidateCurrentResult();
	void calibrate();
public:
	proximityRing();
	void pollRing();
	proximity_target checkTarget(uint8_t sensor_index);
	proximity_target currentTarget();
	void testChannel(uint8_t index);
};

#endif

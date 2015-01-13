#ifndef ADXL345_H
#define ADXL345_H

#include <iostream>

typedef enum {
	ADX_X = 0,
	ADX_Y,
	ADX_z
} ADX_Axis;

class ADXL345 {
public:
	ADXL345();
	~ADXL345();
	void init();
	double readAxis(ADX_Axis axis);

private:
	int i2cDescriptor;
	bool busReady;
	bool sensorReady;

	bool initI2CBus();
	inline bool writeByte(uint8_t byte);
	inline bool write2Byte(uint8_t byte1, uint8_t byte2);
};

#endif
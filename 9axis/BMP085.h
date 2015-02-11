#ifndef BMP085_H
#define BMP085_H

#include <iostream>

/**
 * @brief Just gonna leave this here since we don't need pressure and temp yet
 * @details [long description]
 * @return [description]
 */
class BMP085 {
public:
	BMP085();
	~BMP085();
	void init();

private:
	int i2cDescriptor;
	bool busReady;
	bool sensorReady;

	// Calibration values
	int ac1;
	int ac2;
	int ac3;
	unsigned int ac4;
	unsigned int ac5;
	unsigned int ac6;
	int b1;
	int b2;
	int mb;
	int mc;
	int md;

	bool initI2CBus();
	inline bool writeByte(uint8_t byte);
	inline bool write2Byte(uint8_t byte1, uint8_t byte2);
};

#endif
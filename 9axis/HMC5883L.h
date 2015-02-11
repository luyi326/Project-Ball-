#ifndef HMC5883L_H
#define HMC5883L_H

#include <iostream>

struct MagnetometerScaled
{
	float XAxis;
	float YAxis;
	float ZAxis;
};

struct MagnetometerRaw
{
	int XAxis;
	int YAxis;
	int ZAxis;
};

class HMC5883L {
public:
	HMC5883L();
	~HMC5883L();

	MagnetometerRaw ReadRawAxis();
	MagnetometerScaled ReadScaledAxis();

	void setMeasurementMode(uint8_t mode);
	void setScale(float gauss);
private:
	int i2cDescriptor;
	bool busReady;
	bool sensorReady;

	float m_Scale;

	bool initI2CBus();
	inline bool writeToAddress(uint8_t address, uint8_t byte);
	inline bool writeByte(uint8_t byte);
	inline bool write2Byte(uint8_t byte1, uint8_t byte2);
};

#endif
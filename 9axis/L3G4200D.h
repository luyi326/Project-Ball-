#ifndef L3G4200D_H
#define L3G4200D_H

#include <iostream>

class L3G4200D {
public:
	typedef struct vector
	{
		float x, y, z;
	} vector;

	vector g;
	L3G4200D();
	~L3G4200D();
	void init();
	void sample();

private:
	int i2cDescriptor;
	bool busReady;
	bool sensorReady;

	bool initI2CBus();
	inline bool writeByte(uint8_t byte);
	inline bool write2Byte(uint8_t byte1, uint8_t byte2);
};

#endif
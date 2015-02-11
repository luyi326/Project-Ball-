#ifndef I2C_BASE_H
#define I2C_BASE_H

#include <iostream>
#include <stdint.h>

using namespace std;

class I2CBase {

public:
	I2CBase(string busName, uint8_t address);
	~I2CBase();
	bool isBusReady();
	uint8_t read(uint8_t reg);
	void writeReg(uint8_t reg, uint8_t value);
	void write(uint8_t value);
protected:
	uint8_t address;
private:
	// int openConnection();
	string busName;
	int i2cDescriptor;
	bool busReady;
};

#endif

#include "I2CBase.h"
#include <iostream>
#include <cstdint>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

using namespace std;

I2CBase::I2CBase(string _busName, uint8_t _address) {
	busName = _busName;
	address = _address;
	i2cDescriptor = open(busName, O_RDWR);
	if (i2cDescriptor < 0) {
		cout << "I2CBase::I2CBase::Open i2c bus " << busName;
		cout << " error, error message: " << strerror(errno) << endl;
		return false;
	}
	if (ioctl(i2cDescriptor, I2C_SLAVE, address) < 0) {
		cout << "I2CBase::I2CBase::Init slave " << busName;
		cout << "error, error message: " << strerror(errno) << endl;
		return false;
	}
	busReady = true;
	return true;
}

I2CBase::~I2CBase() {
	if (close(i2cDescriptor) == -1) {
		cout << "I2CBase::~I2CBase:: write fail";
		cout << ", error message: " << strerror(errno) << endl;
	}
}

bool I2CBase::isBusReady() {
	return busReady;
}

uint8_t I2CBase::read(uint8_t reg) {

	if (write(file, &reg, 1) != 1) {
		cout << "Can not write data. Address " << address << "." << endl;
		return 0;
	}

	uint8_t value;

	if (read(file, &value, 1) != 1) {
		cout << "Can not read data. Address " << address << "." << endl;
		return 0;
	}
	return value;
}

void I2CBase::writeReg(uint8_t reg, uint8_t value) {
	uint8_t buffer[2] = {reg, value};
	if (write(file, buffer, 2) != 2) {
		cout << "Can not write data. Address " << address << "." << endl;
	}
}

void write(uint8_t value) {
	if (write(file, &value, 1) != 1) {
		cout << "Can not write data. Address " << address << "." << endl;
	}
}

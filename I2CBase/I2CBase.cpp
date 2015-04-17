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

I2CBase::I2CBase(string _busName, uint8_t _address) : address(_address), busName(_busName) {
	// busName = _busName;
	// address = _address;
	i2cDescriptor = open(busName.c_str(), O_RDWR);
	busReady = false;
	if (i2cDescriptor < 0) {
		cerr << "I2CBase::I2CBase::Open i2c bus " << busName;
		cerr << " error, error message: " << strerror(errno) << endl;
		return;
	}
	if (ioctl(i2cDescriptor, I2C_SLAVE, address) < 0) {
		cerr << "I2CBase::I2CBase::Init slave " << busName;
		cerr << "error, error message: " << strerror(errno) << endl;
		return;
	}
	busReady = true;
}

I2CBase::~I2CBase() {
	if (close(i2cDescriptor) == -1) {
		cerr << "I2CBase::~I2CBase:: write fail";
		cerr << ", error message: " << strerror(errno) << endl;
	}
}

bool I2CBase::isBusReady() {
	return busReady;
}

uint8_t I2CBase::readByte(uint8_t reg) {
	if (!busReady) {
		cerr << "Bus not ready!" << endl;
		return 0;
	}

	if (write(i2cDescriptor, &reg, 1) != 1) {
		cerr << "Can not write data. Address 0x" << hex << (int)address << "." << dec << endl;
		return 0;
	}

	uint8_t value;

	if (read(i2cDescriptor, &value, 1) != 1) {
		cerr << "Can not read data. Address 0x" << hex << (int)address << "." << dec << endl;
		return 0;
	}
	return value;
}

void I2CBase::writeReg(uint8_t reg, uint8_t value) {
	if (!busReady) {
		cerr << "Bus not ready!" << endl;
		return;
	}

	uint8_t buffer[2] = {reg, value};
	if (write(i2cDescriptor, buffer, 2) != 2) {
		cerr << "Can not write data. Address 0x" << hex << (int)address << "." << dec << endl;
	}
}

void I2CBase::writeByte(uint8_t value) {
	if (!busReady) {
		cerr << "Bus not ready!" << endl;
		return;
	}

	if (write(i2cDescriptor, &value, 1) != 1) {
		cerr << "Can not write data. Address 0x" << hex << (int)address << "." << dec << endl;
	}
}

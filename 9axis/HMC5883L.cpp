#include "HMC5883L.h"
// #include <chrono>
// #include <cstdio>
#include <cmath>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

using namespace std;

#define EPSILON 0.00001
#define ConfigurationRegisterA 0x00
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define DataRegisterBegin 0x03

#define Measurement_Continuous 0x00
#define Measurement_SingleShot 0x01
#define Measurement_Idle 0x03

// #define ErrorCode_1 "Entered scale was not valid, valid gauss values are: 0.88, 1.3, 1.9, 2.5, 4.0, 4.7, 5.6, 8.1"
// #define ErrorCode_1_Num 1

#define HMC_ADDR (0x3C >> 1) //Essentially 0x1E
#define I2C_BUS_NAME "/dev/i2c-1"

HMC5883L::HMC5883L() : busReady(false), sensorReady(false) {
	initI2CBus();
}

HMC5883L::~HMC5883L() {
	if (close(i2cDescriptor) == -1) {
		cout << "HMC5883L::~HMC5883L:: write fail";
		cout << ", error message: " << strerror(errno) << endl;
	}
}
void HMC5883L::setMeasurementMode(uint8_t mode) {
	writeToAddress(ModeRegister, mode);
}

MagnetometerRaw ReadRawAxis() {
    bool writeResult = writeByte(DataRegisterBegin);
    MagnetometerRaw raw = MagnetometerRaw();
    if (!writeResult) {
        cout << "HMC5883L::ReadRawAxis::write fail" << endl;
        return raw;
    }
    uint8_t buffer[6];
    int readResult = read(i2cDescriptor, buffer, 6);
    if (readResult < 6) {
        cout << "HMC5883L::ReadRawAxis::read less than 6 bits, read " << readResult << " bits" << endl;
        return raw;
    }
    raw.XAxis = (buffer[0] << 8) | buffer[1];
    raw.ZAxis = (buffer[2] << 8) | buffer[3];
    raw.YAxis = (buffer[4] << 8) | buffer[5];
    return raw;
}

MagnetometerScaled ReadScaledAxis() {
    MagnetometerRaw raw = ReadRawAxis();
    MagnetometerScaled scaled = MagnetometerScaled();
    scaled.XAxis = raw.XAxis * m_Scale;
    scaled.ZAxis = raw.ZAxis * m_Scale;
    scaled.YAxis = raw.YAxis * m_Scale;
    return scaled;
}

void HMC5883L::setScale(float gauss) {
	uint8_t regValue = 0x00;
	if(fabs(gauss - 0.88) < EPSILON)
	{
		regValue = 0x00;
		m_Scale = 0.73;
	}
	else if(fabs(gauss - 1.3) < EPSILON)
	{
		regValue = 0x01;
		m_Scale = 0.92;
	}
	else if(fabs(gauss - 1.9) < EPSILON)
	{
		regValue = 0x02;
		m_Scale = 1.22;
	}
	else if(fabs(gauss - 2.5) < EPSILON)
	{
		regValue = 0x03;
		m_Scale = 1.52;
	}
	else if(fabs(gauss - 4.0) < EPSILON)
	{
		regValue = 0x04;
		m_Scale = 2.27;
	}
	else if(fabs(gauss - 4.7) < EPSILON)
	{
		regValue = 0x05;
		m_Scale = 2.56;
	}
	else if(fabs(gauss - 5.6) < EPSILON)
	{
		regValue = 0x06;
		m_Scale = 3.03;
	}
	else if(fabs(gauss - 8.1) < EPSILON)
	{
		regValue = 0x07;
		m_Scale = 4.35;
	}
	else
		return ErrorCode_1_Num;

	// Setting is in the top 3 bits of the register.
	regValue <<= 5;
	writeToAddress(ConfigurationRegisterB, regValue);
}

/**
 * @brief Initialize I2C bus
 *
 * @details Initialize I2C bus with linux file descriptor
 *
 * @return true on bus open success, false on failure
 */
bool HMC5883L::initI2CBus() {
	i2cDescriptor = open(I2C_BUS_NAME, O_RDWR);
	if (i2cDescriptor < 0) {
		cout << "HMC5883L::initI2CBus::Open i2c bus " << I2C_BUS_NAME;
		cout << " error, error message: " << strerror(errno) << endl;
		return false;
	}
	if (ioctl(i2cDescriptor, I2C_SLAVE, HMC_ADDR) < 0) {
		cout << "HMC5883L::initI2CBus::Init slave " << HMC_ADDR;
		cout << "error, error message: " << strerror(errno) << endl;
		return false;
	}
	busReady = true;
	// usleep(100000);
	return true;
}

inline bool HMC5883L::writeByte(uint8_t byte) {
	if (!busReady) {
		cout << "HMC5883L::writeByte:: Bus not ready, doing nothing" << endl;
		return false;
	}
	if (write(i2cDescriptor, &byte, 1) == -1) {
		cout << "HMC5883L::writeByte:: write fail";
		cout << ", error message: " << strerror(errno) << endl;
		return false;
	}
	return true;
}

inline bool HMC5883L::write2Byte(uint8_t d1, uint8_t d2) {
	if (!busReady) {
		cout << "HMC5883L::write2Byte:: Bus not ready, doing nothing" << endl;
		return false;
	}
	char word[2];
	word[0] = d1;
	word[1] = d2;
	if (write(i2cDescriptor, word, 2) == -1) {
		cout << "HMC5883L::write2Byte:: write fail";
		cout << ", error message: " << strerror(errno) << endl;
		return false;
	}
	return true;
}

inline bool HMC5883L::writeToAddress(uint8_t address, uint8_t byte) {
	return write2Byte(address, byte);
}
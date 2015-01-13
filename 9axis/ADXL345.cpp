#include "ADXL345.h"
#include <chrono>
#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

using namespace std;

#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37
#define ADX_ADDR (0xA7 >> 1) //Essentially 0x53
#define I2C_BUS_NAME "/dev/i2c-1"

ADXL345::ADXL345() : busReady(false), sensorReady(false) {
    initI2CBus();
}

ADXL345::~ADXL345() {
    if (close(i2cDescriptor) == -1) {
        cout << "ADXL345::~ADXL345:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
    }
}

void ADXL345::init() {
    if (!busReady) {
        cout << "ADXL345::init:: Bus not ready, doing nothing" << endl;
        return;
    }

    bool writeResult = true;
    writeResult &= writeByte(Register_2D);
    writeResult &= writeByte(0x08);
    sensorReady = writeResult;
}

/**
 * @brief Read accelerations on axis
 *
 * @details Read acceleration value on the axis provided by an enum
 * parameter axis
 *
 * @param axis An enum defined in header, three possible values: ADX_X,
 * ADX_Y, ADX_Z for three axises
 *
 * @return normalized result of acceleration on that axis, double
 */
double ADXL345::readAxis(ADX_Axis axis) {
	uint16_t result = 0;
    bool writeResult = true;
	switch (axis) {
		case ADX_X:
			writeResult &= write2Byte(Register_X0, Register_X1);
			break;
		case ADX_Y:
			writeResult &= write2Byte(Register_Y0, Register_Y1);
			break;
		case ADX_Z:
			writeResult &= write2Byte(Register_Z0, Register_Z1);
			break;
		case default:
			cout << "ADXL345::readAxis::Unrecognized axis: " << axis << endl;
			writeResult = false;
			break;
	}
	uint8_t results[2];
	results[0] = 0;
	results[1] = 0;
    int readResult = read(i2cDescriptor, results, 2);
    if (readResult < 2) {
        cout << "ADXL345::readAxis:: read fail, read " << readResult << " bytes";
        cout << ", error message: " << strerror(errno) << endl;
        return ((double)result) / 256.0;
    }
    result = results[0];
    result <<= 8;
    result += results[1];
    return ((double)result) / 256.0;
}

/**
 * @brief Initialize I2C bus
 *
 * @details Initialize I2C bus with linux file descriptor
 *
 * @return true on bus open success, false on failure
 */
bool ADXL345::initI2CBus() {
    i2cDescriptor = open(I2C_BUS_NAME, O_RDWR);
    if (i2cDescriptor < 0) {
        cout << "ADXL345::initI2CBus::Open i2c bus " << I2C_BUS_NAME;
        cout << " error, error message: " << strerror(errno) << endl;
        return false;
    }
    if (ioctl(i2cDescriptor, I2C_SLAVE, ADX_ADDR) < 0) {
        cout << "ADXL345::initI2CBus::Init slave " << ADX_ADDR;
        cout << "error, error message: " << strerror(errno) << endl;
        return false;
    }
    busReady = true;
    usleep(100000);
    return true;
}

inline bool ADXL345::writeByte(uint8_t byte) {
    if (!busReady) {
        cout << "ADXL345::writeByte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    if (write(i2cDescriptor, &byte, 1) == -1) {
        cout << "ADXL345::writeByte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}

inline bool ADXL345::write2Byte(uint8_t d1, uint8_t d2) {
    if (!busReady) {
        cout << "ADXL345::write2Byte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    char word[2];
    word[0] = d1;
    word[1] = d2;
    if (write(i2cDescriptor, word, 2) == -1) {
        cout << "ADXL345::write2Byte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}
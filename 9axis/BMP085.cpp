#include "BMP085.h"
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

#define ADX_ADDR (0xEE >> 1) //Essentially 0x77
#define I2C_BUS_NAME "/dev/i2c-1"

BMP085::BMP085() : busReady(false), sensorReady(false) {
    initI2CBus();
}

BMP085::~BMP085() {
    if (close(i2cDescriptor) == -1) {
        cout << "BMP085::~BMP085:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
    }
}

void BMP085::init() {
    if (!busReady) {
        cout << "BMP085::init:: Bus not ready, doing nothing" << endl;
        return;
    }

    bool writeResult = true;
    writeResult &= writeByte(Register_2D);
    writeResult &= writeByte(0x08);
    sensorReady = writeResult;
}

bool BMP085::initI2CBus() {
    i2cDescriptor = open(I2C_BUS_NAME, O_RDWR);
    if (i2cDescriptor < 0) {
        cout << "BMP085::initI2CBus::Open i2c bus " << I2C_BUS_NAME;
        cout << " error, error message: " << strerror(errno) << endl;
        return false;
    }
    if (ioctl(i2cDescriptor, I2C_SLAVE, ADX_ADDR) < 0) {
        cout << "BMP085::initI2CBus::Init slave " << ADX_ADDR;
        cout << "error, error message: " << strerror(errno) << endl;
        return false;
    }
    busReady = true;
    usleep(100000);
    return true;
}

inline bool BMP085::writeByte(uint8_t byte) {
    if (!busReady) {
        cout << "BMP085::writeByte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    if (write(i2cDescriptor, &byte, 1) == -1) {
        cout << "BMP085::writeByte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}

inline bool BMP085::write2Byte(uint8_t d1, uint8_t d2) {
    if (!busReady) {
        cout << "BMP085::write2Byte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    char word[2];
    word[0] = d1;
    word[1] = d2;
    if (write(i2cDescriptor, word, 2) == -1) {
        cout << "BMP085::write2Byte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}
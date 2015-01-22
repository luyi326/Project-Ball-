#include "L3G4200D.h"
// #include <chrono>
// #include <cstdio>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

using namespace std;

// register addresses

#define L3G4200D_WHO_AM_I      0x0F

#define L3G4200D_CTRL_REG1     0x20
#define L3G4200D_CTRL_REG2     0x21
#define L3G4200D_CTRL_REG3     0x22
#define L3G4200D_CTRL_REG4     0x23
#define L3G4200D_CTRL_REG5     0x24
#define L3G4200D_REFERENCE     0x25
#define L3G4200D_OUT_TEMP      0x26
#define L3G4200D_STATUS_REG    0x27

#define L3G4200D_OUT_X_L       0x28
#define L3G4200D_OUT_X_H       0x29
#define L3G4200D_OUT_Y_L       0x2A
#define L3G4200D_OUT_Y_H       0x2B
#define L3G4200D_OUT_Z_L       0x2C
#define L3G4200D_OUT_Z_H       0x2D

#define L3G4200D_FIFO_CTRL_REG 0x2E
#define L3G4200D_FIFO_SRC_REG  0x2F

#define L3G4200D_INT1_CFG      0x30
#define L3G4200D_INT1_SRC      0x31
#define L3G4200D_INT1_THS_XH   0x32
#define L3G4200D_INT1_THS_XL   0x33
#define L3G4200D_INT1_THS_YH   0x34
#define L3G4200D_INT1_THS_YL   0x35
#define L3G4200D_INT1_THS_ZH   0x36
#define L3G4200D_INT1_THS_ZL   0x37
#define L3G4200D_INT1_DURATION 0x38

#define L3G_ADDR (0xD2 >> 1) //Essentially 0x69
#define I2C_BUS_NAME "/dev/i2c-1"

L3G4200D::L3G4200D() : busReady(false), sensorReady(false) {
    initI2CBus();
}

L3G4200D::~L3G4200D() {
    if (close(i2cDescriptor) == -1) {
        cout << "L3G4200D::~L3G4200D:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
    }
}

void L3G4200D::init() {
    if (!busReady) {
        cout << "L3G4200D::init:: Bus not ready, doing nothing" << endl;
        return;
    }

    sensorReady = write2Byte(L3G4200D_CTRL_REG1, 0x0F);
}

void L3G4200D::sample() {
    // assert the MSB of the address to get the gyro
    // to do slave-transmit subaddress updating.
    writeByte(L3G4200D_OUT_X_L | (1 << 7));
    uint8_t buffer[6];
    int readResult = read(i2cDescriptor, buffer, 6);
    if (readResult < 6) {
        cout << "L3G4200D::sample::read less than 6 bits, read " << readResult << " bits" << endl;
        return;
    }
    // for (int i = 0; i < 6; i++) {
    //  cout << hex << (int)buffer[i] << ":";
    // }
    // cout << endl;

    uint8_t xla = buffer[0];
    uint8_t xha = buffer[1];
    uint8_t yla = buffer[2];
    uint8_t yha = buffer[3];
    uint8_t zla = buffer[4];
    uint8_t zha = buffer[5];

    g.x = xha << 8 | xla;
    g.y = yha << 8 | yla;
    g.z = zha << 8 | zla;
}

bool L3G4200D::initI2CBus() {
    i2cDescriptor = open(I2C_BUS_NAME, O_RDWR);
    if (i2cDescriptor < 0) {
        cout << "L3G4200D::initI2CBus::Open i2c bus " << I2C_BUS_NAME;
        cout << " error, error message: " << strerror(errno) << endl;
        return false;
    }
    if (ioctl(i2cDescriptor, I2C_SLAVE, L3G_ADDR) < 0) {
        cout << "L3G4200D::initI2CBus::Init slave " << L3G_ADDR;
        cout << "error, error message: " << strerror(errno) << endl;
        return false;
    }
    busReady = true;
    usleep(100000);
    return true;
}

inline bool L3G4200D::writeByte(uint8_t byte) {
    if (!busReady) {
        cout << "L3G4200D::writeByte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    if (write(i2cDescriptor, &byte, 1) == -1) {
        cout << "L3G4200D::writeByte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}

inline bool L3G4200D::write2Byte(uint8_t d1, uint8_t d2) {
    if (!busReady) {
        cout << "L3G4200D::write2Byte:: Bus not ready, doing nothing" << endl;
        return false;
    }
    char word[2];
    word[0] = d1;
    word[1] = d2;
    if (write(i2cDescriptor, word, 2) == -1) {
        cout << "L3G4200D::write2Byte:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return false;
    }
    return true;
}
// Example of using the PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com
/******************************************************************************
* Includes
******************************************************************************/
#include "PVision.h"
#include <chrono>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define SENSOR_ADDRESS 0x58

#define I2C_BUS_NAME "/dev/i2c-1".c_str()

/******************************************************************************
* Private methods
******************************************************************************/
void PVision::Write_2bytes(uint8_t d1, uint8_t d2)
{
    if (!busReady) {
        cout << "PVision::Write_2bytes:: Bus not ready, doing nothing" << endl;
        return;
    }
    char* word = char[2];
    word[0] = d1;
    word[1] = d2;
    if (write(i2cDescriptor, word, 2) != 1) {
        cout << "PVision::Write_2bytes:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
    }
}

bool PVision::initI2CBus() {
    i2cDescriptor = open(I2C_BUS_NAME, O_RDWR);
    if (i2cDescriptor < 0) {
        cout << "PVision::initI2CBus::Open i2c bus " << I2C_BUS_NAME;
        cout << " error, error message: " << strerror(errno) << endl;
        return false;
    }
    if (ioctl(i2cDescriptor, I2C_SLAVE, SENSOR_ADDRESS) < 0) {
        cout << "PVision::initI2CBus::Init slave " << SENSOR_ADDRESS;
        cout << "error, error message: " << strerror(errno) << endl;
        return false;
    }
    busReady = true;
    return true;
}

/******************************************************************************
* Constructor
******************************************************************************/
PVision::PVision() : busReady(false)
{
	Blob1.number = 1;
	Blob2.number = 2;
	Blob3.number = 3;
	Blob4.number = 4;
    initI2CBus();
}

/******************************************************************************
* Destructor
******************************************************************************/
PVision::~PVision() {
    close(i2cDescriptor);
}

/******************************************************************************
* Public methods
******************************************************************************/
// init the PVision sensor
void PVision::init ()
{
    if (!busReady) {
        cout << "PVision::init:: Bus not ready, doing nothing" << endl;
        return;
    }
    // IRsensorAddress = SENSOR_ADDRESS;
    // IRslaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI

    // Wire.begin();
    // IR sensor initialize
    Write_2bytes(0x30,0x01); usleep(10000);
    Write_2bytes(0x30,0x08); usleep(10000);
    Write_2bytes(0x06,0x90); usleep(10000);
    Write_2bytes(0x08,0xC0); usleep(10000);
    Write_2bytes(0x1A,0x40); usleep(10000);
    Write_2bytes(0x33,0x33); usleep(10000);
    usleep(100000);
}

bool PVision::isBusReady() {
    return busReady;
}

uint8_t PVision::read()
{
    if (!busReady) {
        cout << "PVision::read:: Bus not ready, doing nothing" << endl;
        return;
    }
    char requestByte = 0x36;
    if (write(i2cDescriptor, &requestByte, 1) != 1) {
        cout << "PVision::read:: write fail";
        cout << ", error message: " << strerror(errno) << endl;
        return 0xFF;
    }

    // Wire.requestFrom(IRslaveAddress, 16);        // Request the 2 byte heading (MSB comes first)

    for (i=0;i<16;i++)
    {
       data_buf[i]=0;
    }

    i=0;

    // while(Wire.available() && i < 16)
    // {
    //     data_buf[i] = Wire.receive();
    //     i++;
    // }

    // while(i < 16)
    // {
    //     data_buf[i] = i2c.readByte(IRslaveAddress);
    //     i++;
    // }
    if (read(i2cDescriptor, data_buf, 16) < 16) {
        cout << "PVision::read:: read fail";
        cout << ", error message: " << strerror(errno) << endl;
        return 0xFF;
    }

    blobcount = 0;

    Blob1.X = data_buf[1];
    Blob1.Y = data_buf[2];
    s   = data_buf[3];
    Blob1.X += (s & 0x30) <<4;
    Blob1.Y += (s & 0xC0) <<2;
    Blob1.Size = (s & 0x0F);

    // At the moment we're using the size of the blob to determine if one is detected, either X,Y, or size could be used.
    blobcount |= (Blob1.Size < 15)? BLOB1 : 0;

    Blob2.X = data_buf[4];
    Blob2.Y = data_buf[5];
    s   = data_buf[6];
    Blob2.X += (s & 0x30) <<4;
    Blob2.Y += (s & 0xC0) <<2;
    Blob2.Size = (s & 0x0F);

    blobcount |= (Blob2.Size < 15)? BLOB2 : 0;

    Blob3.X = data_buf[7];
    Blob3.Y = data_buf[8];
    s   = data_buf[9];
    Blob3.X += (s & 0x30) <<4;
    Blob3.Y += (s & 0xC0) <<2;
    Blob3.Size = (s & 0x0F);

    blobcount |= (Blob3.Size < 15)? BLOB3 : 0;

    Blob4.X = data_buf[10];
    Blob4.Y = data_buf[11];
    s   = data_buf[12];
    Blob4.X += (s & 0x30) <<4;
    Blob4.Y += (s & 0xC0) <<2;
    Blob4.Size = (s & 0x0F);

    blobcount |= (Blob4.Size < 15)? BLOB4 : 0;

    return blobcount;
}


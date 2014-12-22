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

#define SENSOR_ADDRESS 0xB0

/******************************************************************************
* Private methods
******************************************************************************/
void PVision::Write_2bytes(uint8_t d1, uint8_t d2)
{
    // Wire.beginTransmission(IRslaveAddress);
    // Wire.send(d1); Wire.send(d2);
    // Wire.endTransmission();
    i2c.writeByte(IRslaveAddress, d1);
    i2c.writeByte(IRslaveAddress, d2);
}


/******************************************************************************
* Constructor
******************************************************************************/
PVision::PVision(i2cName port) : i2c(port, SENSOR_ADDRESS)
{
	Blob1.number = 1;
	Blob2.number = 2;
	Blob3.number = 3;
	Blob4.number = 4;
    i2c.open(ReadWrite | NonBlock);
}

/******************************************************************************
* Destructor
******************************************************************************/
PVision::~PVision() {
    i2c.close();
}

/******************************************************************************
* Public methods
******************************************************************************/
// init the PVision sensor
void PVision::init ()
{
    IRsensorAddress = SENSOR_ADDRESS;
    IRslaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI

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

uint8_t PVision::read()
{
    //IR sensor read
    // Wire.beginTransmission(IRslaveAddress);
    // Wire.send(0x36);
    // Wire.endTransmission();
    i2c.writeByte(IRslaveAddress, 0x36);

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

    while(i < 16)
    {
        data_buf[i] = i2c.readByte(IRslaveAddress);
        i++;
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


// PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com
//
// Yi Lu 2014, Modified for Beaglebone Black

#ifndef PVision_h
#define PVision_h

#include <iostream>
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackI2C.h"

// bit flags for blobs
#define BLOB1 0x01
#define BLOB2 0x02
#define BLOB3 0x04
#define BLOB4 0x08


// Returned structure from a call to readSample()
struct Blob
{
   	int X;
   	int Y;
   	int Size;
   	uint8_t number;
};

class PVision
{

public:
  	PVision(i2cName port);
  	~PVision();

	void init();   // returns true if the connection to the sensor established correctly
	uint8_t read();   // updated the blobs, and returns the number of blobs detected

	// Make these public
	Blob Blob1;
	Blob Blob2;
	Blob Blob3;
	Blob Blob4;

private:
  	// per object data
	int IRsensorAddress;
	int IRslaveAddress;
	uint8_t data_buf[16];
	int i;
	int s;
	BlackI2C i2c;

	void Write_2bytes(uint8_t d1, uint8_t d2);
	uint8_t blobcount; // returns the number of blobs found - reads the sensor

};

#endif

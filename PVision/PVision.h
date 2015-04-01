// PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com
//
// Yi Lu 2014, Modified for Beaglebone Black

#ifndef PVision_h
#define PVision_h

#include <iostream>
using namespace std;

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

bool Blob_is_valid(Blob& b);
ostream& operator<<(ostream& os, const Blob& b);

typedef struct BlobCluster_t {
	Blob first;
	Blob second;
	Blob third;
	Blob forth;
	int validBlobCount;
} BlobCluster;

class PVision
{

public:
  	PVision();
  	~PVision();

  	bool isBusReady();
  	bool isSensorReady();
	bool init();   // returns true if the connection to the sensor established correctly
	uint8_t readBlob();   // updated the blobs, and returns the number of blobs detected
	void reset();

	// Make these public
	Blob Blob1;
	Blob Blob2;
	Blob Blob3;
	Blob Blob4;

private:
	int i2cDescriptor;

  	// per object data
	// int IRsensorAddress;
	// int IRslaveAddress;
	uint8_t data_buf[16];
	int i;
	int s;

	bool Write_2bytes(uint8_t d1, uint8_t d2);
	uint8_t blobcount; // returns the number of blobs found - reads the sensor

	bool initI2CBus();
	bool busReady;
	bool sensorReady;
};

#endif

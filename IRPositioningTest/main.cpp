#include <iostream>
#include <unistd.h>
#include <ctime>
#include "../PVision/PCA9548A.h"
#include "../PVision/PVision.h"

#define IR_DISTANCE (8.1) //This is in cms

using namespace std;
using namespace BlackLib;

int main (int argc, char* argv[]) {
	PCA9548A mux(GPIO_48);
	PVision v1, v2;
	mux.selectChannel(0x04);
	v1.init();
	mux.selectChannel(0x08);
	v2.init();
	cout << std::dec;

	while (1) {
		// timespec start;
		// clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		mux.selectChannel(0x04);
		uint8_t result = v1.readBlob();
		bool resultOne = false, resultTwo = false;
		if (result & BLOB1)
		{
			resultOne = true;
			// cout << "v1 BLOB1 detected. X:" << v1.Blob1.X << " Y:" << v1.Blob1.Y;
			// cout << " Size: " << v1.Blob1.Size << endl;
		}
		mux.selectChannel(0x08);
		result = v2.readBlob();
		if (result & BLOB1)
		{
			resultTwo = true;
			// cout << "v2 BLOB1 detected. X:" << v2.Blob1.X << " Y:" << v2.Blob1.Y;
			// cout << " Size: " << v2.Blob1.Size << endl;
		}

		if (resultTwo/* && resultTwo*/) {
			// cout << "v1 BLOB1 detected. X:" << v1.Blob1.X << " Y:" << v1.Blob1.Y;
			// cout << " Size: " << v1.Blob1.Size << endl;
			cout << "v2 BLOB1 detected. X:" << v2.Blob1.X << " Y:" << v2.Blob1.Y;
			cout << " Size: " << v2.Blob1.Size << endl;

		}
		// timespec stop;
		// clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		// // cout << start << "  " << stop << endl;
		// timespec temp;
		// if ((stop.tv_nsec-start.tv_nsec)<0) {
		// 	temp.tv_sec = stop.tv_sec-start.tv_sec-1;
		// 	temp.tv_nsec = 1000000000+stop.tv_nsec-start.tv_nsec;
		// } else {
		// 	temp.tv_sec = stop.tv_sec-start.tv_sec;
		// 	temp.tv_nsec = stop.tv_nsec-start.tv_nsec;
		// }
		// cout << temp.tv_sec << "." << temp.tv_nsec << endl;
		usleep(5000);

	}
	return 0;
}
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include "../PVision/PCA9548A.h"
#include "../PVision/PVision.h"

using namespace std;
using namespace BlackLib;

string ZeroPadNumber(int num)
{
	stringstream ss;

	// the number is converted to string with the help of stringstream
	ss << num;
	string ret;
	ss >> ret;

	// Append zero chars
	int str_length = ret.length();
	for (int i = 0; i < 9 - str_length; i++)
		ret = "0" + ret;
	return ret;
}

int main (int argc, char* argv[]) {
	PCA9548A mux(GPIO_48);
	PVision v1, v2;
    unsigned int number = 0x00;
    if (argc >= 2) {
        number = atoi(argv[1]);
    }
	mux.selectChannel(number);
	v1.init();
	// mux.selectChannel(0x08);
	// v2.init();

	while (1) {
		// cout << "Try read blob" << endl;
		timespec start;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		mux.selectChannel(number);
		uint8_t result = v1.readBlob();
		// cout << "Raw result = " << result << endl;
		cout << std::dec;
		if (result & BLOB1)
		{
			cout << "v1 BLOB1 detected. X:" << v1.Blob1.X << " Y:" << v1.Blob1.Y;
			cout << " Size: " << v1.Blob1.Size << endl;
		}
		// mux.selectChannel(0x08);
		// result = v2.readBlob();
		// // cout << "Raw result = " << result << endl;
		// // cout << std::dec;
		if (result & BLOB2)
		{
			cout << "v1 BLOB2 detected. X:" << v1.Blob1.X << " Y:" << v1.Blob1.Y;
			cout << " Size: " << v1.Blob1.Size << endl;
		}

		// if (result & BLOB2)
		// {
		// 	cout << "BLOB2 detected. X:" << v1.Blob2.X << " Y:" << v1.Blob2.Y;
		// 	cout << " Size: " << v1.Blob2.Size << endl;
		// }
		// if (result & BLOB3)
		// {
		// 	cout << "BLOB3 detected. X:" << v1.Blob3.X << " Y:" << v1.Blob3.Y;
		// 	cout << " Size: " << v1.Blob3.Size << endl;
		// }
		// if (result & BLOB4)
		// {
		// 	cout << "BLOB4 detected. X:" << v1.Blob4.X << " Y:" << v1.Blob4.Y;
		// 	cout << " Size: " << v1.Blob4.Size << endl;
		// }
		timespec stop;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		// cout << start << "  " << stop << endl;
		timespec temp;
		if ((stop.tv_nsec-start.tv_nsec)<0) {
			temp.tv_sec = stop.tv_sec-start.tv_sec-1;
			temp.tv_nsec = 1000000000+stop.tv_nsec-start.tv_nsec;
		} else {
			temp.tv_sec = stop.tv_sec-start.tv_sec;
			temp.tv_nsec = stop.tv_nsec-start.tv_nsec;
		}
		cout << temp.tv_sec << "." << ZeroPadNumber(temp.tv_nsec) << endl;
		usleep(5000);

	}
	return 0;
}
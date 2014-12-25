#include <iostream>
#include <unistd.h>
#include "../PVision/Pvision.h"

using namespace std;

int main (int argc, char* argv[]) {
	PVision vision;
	vision.init();
	if (!vision.isBusReady()) {
		cout << "Bus not ready, exit" << endl;
		return 1;
	}

	while (1) {
		uint8_t result = vision.read();

		if (result & BLOB1)
		{
			cout << "BLOB1 detected. X:" << vision.Blob1.X << " Y:" << vision.Blob1.Y;
			cout << "Size: " << vision.Blob1.Size << endl;
		}

		if (result & BLOB2)
		{
			cout << "BLOB2 detected. X:" << vision.Blob2.X << " Y:" << vision.Blob2.Y;
			cout << "Size: " << vision.Blob2.Size << endl;
		}
		if (result & BLOB3)
		{
			cout << "BLOB2 detected. X:" << vision.Blob3.X << " Y:" << vision.Blob3.Y;
			cout << "Size: " << vision.Blob3.Size << endl;
		}
		if (result & BLOB4)
		{
			cout << "BLOB4 detected. X:" << vision.Blob4.X << " Y:" << vision.Blob4.Y;
			cout << "Size: " << vision.Blob4.Size << endl;
		}

	}
	return 0;
}
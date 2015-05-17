#include <iostream>
#include <unistd.h>
#include <ctime>
// #include "../PVision/IRRim.h"
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"
// #include "../BlackLib/BlackPWM.h"

#include "../PVision/PCA9548A.h"
#include "../PVision/PVision.h"


using namespace std;
using namespace BlackLib;

int main (int argc, char* argv[]) {
    int i1 = 0;
    if (argc >= 2) {
        i1 = atoi(argv[1]);
    }
    int i2 = 3;
    if (argc >= 3) {
        i2 = atoi(argv[2]);
    }
    PCA9548A mux(GPIO_48);
    // cout << "mux selecting channel " << int(1 << index) << endl;
    for (int i = i1; i <= i2; i++) {
		try {
		    mux.selectChannel(1 << i);
		    PVision pv1;
			pv1.init();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			pv1.readBlob();
			cout << "pvision " << i << " succeeded to initialize" << endl;
		} catch (...) {
			cerr << "pvision " << i << " failed to initialize" << endl;
			throw;
		}
    }
    PVision pv1;
    PVision pv2;
    PVision pv3;
    PVision pv4;
	// try {
	//     mux.selectChannel(1 << 0);
	//     pv1.init();
	// } catch (...) {
	// 	cerr << "pvision " << 0 << " failed to initialize" << endl;
	// 	throw;
	// }
	// try {
	//     mux.selectChannel(1 << 1);
	//     pv2.init();
	//     pv2.readBlob();
	// } catch (...) {
	// 	cerr << "pvision " << 1 << " failed to initialize" << endl;
	// 	throw;
	// }
	// try {
	//     mux.selectChannel(1 << 2);
	//     pv3.init();
	// } catch (...) {
	// 	cerr << "pvision " << 2 << " failed to initialize" << endl;
	// 	throw;
	// }
	// try {
	//     mux.selectChannel(1 << 3);
	//     pv4.init();
	// } catch (...) {
	// 	cerr << "pvision " << 3 << " failed to initialize" << endl;
	// 	throw;
	// }

	// while (1) {
 //        rim.run();
	// 	usleep(5000);
	// }
	return 0;
}
#ifndef PCA9548A_H
#define PCA9548A_H

#include <iostream>
#include <stdint.h>
#include "../I2CBase/I2CBase.h"

using namespace std;

class PCA9548A : public I2CBase {
public:
	PCA9548A();
	void selectChannel(uint8_t channel);
};

#endif
#ifndef PCA9548A_H
#define PCA9548A_H

#include <iostream>
#include <stdint.h>
#include "../I2CBase/I2CBase.h"
#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackGPIO.h"

using namespace std;
using namespace BlackLib;

class PCA9548A : public I2CBase {
public:
	PCA9548A(gpioName reset_pin);
	void selectChannel(uint8_t channel);
	void reset();
private:
	BlackGPIO reset_pin;
};

#endif
#include "PCA9548A.h"
#include <unistd.h>

#define I2C_BUS_NAME "/dev/i2c-1"
#define MUX_ADDRESS 0x70


PCA9548A::PCA9548A(gpioName reset_pin_name): I2CBase(I2C_BUS_NAME, MUX_ADDRESS), reset_pin(reset_pin_name, output, SecureMode) {
	reset_pin.setValue((digitalValue)0);
	usleep(100);
	reset_pin.setValue((digitalValue)1);
}

PCA9548A::~PCA9548A() {

}

void PCA9548A::selectChannel(uint8_t channel) {
	this->writeByte(channel);
}

void PCA9548A::reset() {
	reset_pin.setValue((digitalValue)0);
	usleep(200);
	reset_pin.setValue((digitalValue)1);
}

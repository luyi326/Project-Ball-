#include "PCA9548A.h"

#define I2C_BUS_NAME "/dev/i2c-1"
#define MUX_ADDRESS 0x70


PCA9548A::PCA9548A(): I2CBase(I2C_BUS_NAME, MUX_ADDRESS) {

}

void PCA9548A::selectChannel(uint8_t channel) {
	this->writeByte(channel);
}

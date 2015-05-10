#include "arduinoConnector.h"
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <cstdio>

using namespace std;


arduinoConnector::arduinoConnector(spiName port) :
spi(port, 8, SpiDefault, 2400000),
reset_enabled(false)
{
	spi.open(ReadWrite);
	usleep(100);
}

arduinoConnector::arduinoConnector(spiName port, gpioName reset_pin_name) :
spi(port, 8, SpiDefault, 2400000),
reset_enabled(true)
{
	spi.open(ReadWrite);
	usleep(100);
	reset_pin = new BlackGPIO(reset_pin_name, output, SecureMode);
	reset();
}

arduinoConnector::~arduinoConnector() {
	if (reset_enabled) {
		delete reset_pin;
	}
}

float arduinoConnector::angleInfomation(arduinoConnectorKalmanAngle axis) {
	float* rtnValue = new float(0);
	uint8_t parts[4];
	switch (axis) {
		case arduinoConnector_KalmanX:
			spi.transfer('x', 10) & 0xFF;
		break;
		case arduinoConnector_KalmanY:
			spi.transfer('y', 10) & 0xFF;
		break;
		case arduinoConnector_KalmanZ:
			spi.transfer('z', 10) & 0xFF;
		break;
		default:
		return 0.0f;
	}
	parts[0] = spi.transfer('1', 10) & 0xFF;
	// printf("Byte 1: 0x%X\n", parts[0] & 0xFF);
	parts[1] = spi.transfer('2', 10) & 0xFF;
	// printf("Byte 2: 0x%X\n", parts[1] & 0xFF);
	parts[2] = spi.transfer('3', 10) & 0xFF;
	// printf("Byte 3: 0x%X\n", parts[2] & 0xFF);
	parts[3] = spi.transfer(0x00, 10) & 0xFF;
	// printf("Byte 4: 0x%X\n", parts[3] & 0xFF);
	int32_t resultInt = parts[3];
	resultInt <<= 8;
	resultInt |= parts[2];
	resultInt <<= 8;
	resultInt |= parts[1];
	resultInt <<= 8;
	resultInt |= parts[0];
	// printf("int: 0x%X\n", resultInt);
	rtnValue = (float*)(&resultInt);
	return *rtnValue;
}

void arduinoConnector::reset() {
	if (!reset_enabled) {
		return;
	}
	reset_pin->setValue((digitalValue)0);
	usleep(200 * 1000);
	reset_pin->setValue((digitalValue)1);
}

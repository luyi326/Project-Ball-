#include "arduinoConnector.h"
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <cstdio>

using namespace std;


arduinoConnector::arduinoConnector(spiName port) : spi(port, 8, SpiDefault, 2400000) {
	spi.open(ReadWrite);
	usleep(100);
	// uint8_t writeData = 'a';
	// for (int i = 0; i < 10; i++) {
	// 	char stuff = spi.transfer(writeData, 1);
	// 	cout << "$" << writeData << "$ $" << stuff << "$" << endl;
	// 	writeData++;
	// 	usleep(100000);
	// }
	// char stuff = spi.transfer('\n', 1);
	// cout << "$" << writeData << "$ $" << stuff << "$" << endl;
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

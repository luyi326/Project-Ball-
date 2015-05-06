#ifndef ARDUINO_CONNECTOR_H
#define ARDUINO_CONNECTOR_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackSPI.h"
#include "../BlackLib/BlackGPIO.h"

using namespace BlackLib;

typedef enum {
	arduinoConnector_KalmanX,
	arduinoConnector_KalmanY,
	arduinoConnector_KalmanZ
} arduinoConnectorKalmanAngle;

class arduinoConnector {
public:
	arduinoConnector(spiName port);
	arduinoConnector(spiName port, gpioName reset);
	float angleInfomation(arduinoConnectorKalmanAngle axis);
	void reset();
private:
	BlackSPI spi;
	BlackGPIO reset_pin;
	bool reset_enabled;
};

#endif

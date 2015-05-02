#ifndef ARDUINO_CONNECTOR_H
#define ARDUINO_CONNECTOR_H

#include "../BlackLib/BlackLib.h"
#include "../BlackLib/BlackSPI.h"

using namespace BlackLib;

typedef enum {
	arduinoConnector_KalmanX,
	arduinoConnector_KalmanY,
	arduinoConnector_KalmanZ
} arduinoConnectorKalmanAngle;

class arduinoConnector {
public:
	arduinoConnector(spiName port);
	float angleInfomation(arduinoConnectorKalmanAngle axis);
	BlackSPI spi;
};

#endif

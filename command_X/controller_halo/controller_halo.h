#ifndef _CONTROLLER_HALO_H_
#define _CONTROLLER_HALO_H_


#include <stdio.h>
#include <stdlib.h>
#include "../../BlackLib/BlackUART.h"
// #include "../BlackLib/BlackCore.h"
#include <stdint.h>
#include <iostream>


typedef struct controller_state_t {
	uint8_t AXIS_0;
	uint8_t AXIS_1;
	uint8_t AXIS_2;
	uint8_t AXIS_3;
	uint8_t AXIS_4;
	uint8_t BUTTON_0;
	uint8_t BUTTON_1;
	uint8_t BUTTON_2;
	uint8_t BUTTON_3;
	uint8_t BUTTON_4;
	uint8_t BUTTON_5;
	uint8_t BUTTON_6;
	uint8_t BUTTON_7;
	uint8_t BUTTON_8;
	uint8_t BUTTON_9;
} controller_state;




enum XBee_state {
	WAITING7E,
	SEVENEREAD,
	FIRSTREAD,
	SECONDREAD
};



class Halo_XBee{
public:
	Halo_XBee();
	void refreshValue();
	controller_state getControllerInfo();
private:
	BlackLib::BlackUART Halo;
	void updateController();
	XBee_state state;
	uint8_t tmpValue[3];
	controller_state controllerInfo;
};

#endif

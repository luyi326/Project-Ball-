#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include "ADXL345.h"


int main(){
	ADXL345 accel;
	accel.init();
	while (1){
		std::cout<<"Now x is: "<<accel.readAxis(ADX_X)<<std::endl;
		// std::cout<<"KAKAK"<<std::endl;
		usleep(100000);
	}
	return 0;
}
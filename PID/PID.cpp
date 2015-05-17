#include "PID.h"

#include <iostream>
using namespace std;

PID::PID(float pg, float ig, float dg, float imax, float imin) {
	dGain = dg;
	iGain = ig;
	pGain = pg;
	iMax = imax;
	iMin = imin;
	dState = 0;
	iState = 0;
}

PID::~PID() {

}

float PID::kernel(float error, float position) {
	float pTerm, dTerm, iTerm;
	pTerm = pGain * error * error; // calculate the proportional term
	if (error < 0) {
		pTerm = -pTerm-1.5;
	}else{
		pTerm+=1.5;
	}
	// calculate the integral state with appropriate limiting
	iState += error;
	if (iState > iMax) iState = iMax;
	else if (iState < iMin) iState = iMin;
	iTerm = iGain * (iState); // calculate the integral term
	dTerm = dGain * (dState - position);
	dState = position;
	// cout << "pTerm: " << pTerm << " iTerm: " << iTerm << " dTerm: " << dTerm << endl;
	return pTerm + dTerm + iTerm;
}

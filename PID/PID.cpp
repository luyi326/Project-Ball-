#include "PID.h"


PID::PID(float ig, float pg, float dg, float imax, float imin) {
	dGain = dg;
	iGain = ig;
	pGain = pg;
	iMax = imax;
	iMin = imin;
}

PID::~PID() {

}

float PID::kernel(float error, float position) {
	float pTerm, dTerm, iTerm;
	pTerm = pGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	iState += error;
	if (iState > iMax) iState = iMax;
	else if (iState < iMin) iState = iMin;
	iTerm = iGain * (iState); // calculate the integral term
	dTerm = dGain * (dState - position);
	dState = position;
	return pTerm + dTerm + iTerm;
}

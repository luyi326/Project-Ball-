#ifndef CAMERA_RING_PID_H
#define CAMERA_RING_PID_H

typedef struct {
	float dState; // Last position input
	float iState; // Integrator state
	float iMax, iMin; // Maximum and minimum allowable integrator stat
	float iGain; // integral gain
	float pGain; // proportional gain
	float dGain; // derivative gain

} PID_cameraRing;

float PID_kernel(PID_cameraRing& pid, float error, float position);
void PID_set(PID_cameraRing& PID_speed, float ig,float pg,float dg, float imax, float imin);


//PID control kernel
float PID_kernel(PID_cameraRing& pid, float error, float position) {
	float pTerm, dTerm, iTerm;
	pTerm = pid.pGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	pid.iState += error;
	if (pid.iState > pid.iMax) pid.iState = pid.iMax;
	else if (pid.iState < pid.iMin) pid.iState = pid.iMin;
	iTerm = pid.iGain * (pid.iState); // calculate the integral term
	dTerm = pid.dGain * (pid.dState - position);
	pid.dState = position;
	return pTerm + dTerm + iTerm;
		//return pTerm + dTerm;
}

void PID_set(PID_cameraRing& PID_speed, float ig,float pg,float dg, float imax, float imin) {
	PID_speed.dGain = dg;
	PID_speed.iGain = ig;
	PID_speed.pGain = pg;
	PID_speed.iMax = imax;
	PID_speed.iMin = imin;

}

#endif
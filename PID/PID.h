#ifndef PID_H
#define PID_H

class PID {
public:
	PID(float ig, float pg, float dg, float imax, float imin);
	~PID();
	float kernel(float error, float position);
private:
	float dState; // Last position input
	float iState; // Integrator state
	float iMax, iMin; // Maximum and minimum allowable integrator stat
	float iGain; // integral gain
	float pGain; // proportional gain
	float dGain; // derivative gain
};

#endif

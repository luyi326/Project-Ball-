#ifndef VEC_H
#define VEC_H

#include <iostream>
using namespace std;

class vec {
public:
	float x;
	float y;
	float z;
	vec();
	vec(float, float, float);
	vec operator+(const vec& v);
	vec operator-(const vec& v);
	vec operator*(const float& f);
	static float dot_product(const vec& v1, const vec& v2);
	static vec mid_point(const vec& v1, const vec& v2);
};

ostream& operator<<(ostream& os, const vec& v);

#endif

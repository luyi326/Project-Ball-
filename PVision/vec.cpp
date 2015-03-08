#include "vec.h"

vec::vec(): x(0), y(0), z(0) {

}

vec::vec(float _x, float _y, float _z): x(_x), y(_y), z(_z) {

}

vec vec::operator+(const vec& v) const {
	vec result;
	result.x = this->x + v.x;
	result.y = this->y + v.y;
	result.z = this->z + v.z;
	return result;
}

vec vec::operator-(const vec& v) const {
	vec result;
	result.x = this->x - v.x;
	result.y = this->y - v.y;
	result.z = this->z - v.z;
	return result;
}

vec vec::operator*(const float& f) const {
	vec result;
	result.x = this->x * f;
	result.y = this->y * f;
	result.z = this->z * f;
	return result;
}

float vec::dot_product(const vec& v1, const vec& v2) {
	float result = 0;
	result += v1.x * v2.x;
	result += v1.y * v2.y;
	result += v1.z * v2.z;
	return result;
}

vec vec::mid_point(const vec& v1, const vec& v2) {
	vec result;
	result.x = (v1.x + v2.x) / 2;
	result.y = (v1.y + v2.y) / 2;
	result.z = (v1.z + v2.z) / 2;
	return result;
}

ostream& operator<<(ostream& os, const vec& v) {
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

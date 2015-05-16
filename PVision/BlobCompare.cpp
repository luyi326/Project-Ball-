#include "BlobCompare.h"
#include <iostream>
#include <cmath>
using namespace std;

#define FULL_HORIZONTAL (1000)
#define FULL_VERTICAL (750)
#define HALF_HORIZONTAL (FULL_HORIZONTAL/2)
#define HALF_VERTICAL (FULL_VERTICAL/2)
#define LEFT_QUARTER_HORIZONTAL (FULL_HORIZONTAL/4)
#define RIGHT_QUARTER_HORIZONTAL (3*FULL_HORIZONTAL/4)

#define FULL_D_SENSOR (4.0f)
#define HALF_D_SENSOR (FULL_D_SENSOR/2)
#define PI (3.14159265)
#define HALF_PI (1.57079632)
#define TAN_16_5 (0.29621349496)
#define TAN_11_5 (0.20345229942)

Blob average(uint8_t result, PVision* pv) {
	if (result & BLOB1) {
		return pv->Blob1;
	}

	Blob avgBlob;
	avgBlob.X = -1;
	avgBlob.Y = -1;
	avgBlob.Size = 1;
	return avgBlob;
	int count = 0;
	if (result & BLOB4) {
		count = 4;
	} else if (result & BLOB3) {
		count = 3;
	} else if (result & BLOB2) {
		count = 2;
	} else if (result & BLOB1) {
		count = 1;
	}
	if (result & BLOB1) {
		avgBlob.X += pv->Blob1.X;
		avgBlob.Y += pv->Blob1.Y;
	}
	if (result & BLOB2) {
		avgBlob.X += pv->Blob2.X;
		avgBlob.Y += pv->Blob2.Y;
	}
	if (result & BLOB3) {
		avgBlob.X += pv->Blob3.X;
		avgBlob.Y += pv->Blob3.Y;
	}
	if (result & BLOB4) {
		avgBlob.X += pv->Blob4.X;
		avgBlob.Y += pv->Blob4.Y;
	}
	if (count == 0) {
		avgBlob.X = -1;
		avgBlob.Y = -1;
		return avgBlob;
	}
	avgBlob.X = int(lround(float(avgBlob.X) / count));
	avgBlob.Y = int(lround(float(avgBlob.Y) / count));
	return avgBlob;
}

float verticalDistance(uint8_t result, PVision* pv) {
	float maxY = 0;
	float minY = 700;
	if (!result & BLOB2) {
		return NAN;
	}
	if (result & BLOB1) {
		if (pv->Blob1.Y > maxY) {
			maxY = pv->Blob1.Y;
		}
		if (pv->Blob1.Y < minY) {
			minY = pv->Blob1.Y;
		}
	}
	if (result & BLOB2) {
		if (pv->Blob2.Y > maxY) {
			maxY = pv->Blob2.Y;
		}
		if (pv->Blob2.Y < minY) {
			minY = pv->Blob2.Y;
		}
	}
	if (result & BLOB3) {
		if (pv->Blob3.Y > maxY) {
			maxY = pv->Blob3.Y;
		}
		if (pv->Blob3.Y < minY) {
			minY = pv->Blob3.Y;
		}
	}
	if (result & BLOB4) {
		if (pv->Blob4.Y > maxY) {
			maxY = pv->Blob4.Y;
		}
		if (pv->Blob4.Y < minY) {
			minY = pv->Blob4.Y;
		}
	}
	float distance = maxY - minY;
	if (distance < 5) {
		return NAN;
	}
	return distance;
}

float running_avg(float distance) {
	static bool initial = true;
	static float avg = 0;
	if (distance < 0) {
		return avg;
	}
	if (initial) {
		initial = false;
		avg = distance;
		return distance;
	} else {
		avg *= 0.99;
		avg += 0.01 * distance;
		if (fabs(avg - distance) > 5) {
			return avg;
		} else {
			return distance;
		}
	}
}

double calculate_target_coordinate(int left_x, int right_x) {
	double alpha = HALF_PI - atan(static_cast<double>(HALF_HORIZONTAL - left_x) / static_cast<double>(HALF_HORIZONTAL) * TAN_16_5);
	double beta = HALF_PI + atan(static_cast<double>(HALF_HORIZONTAL - right_x) / static_cast<double>(HALF_HORIZONTAL) * TAN_16_5);
	double tan_alpha = tan(alpha);
	double tan_beta = tan(beta);
	double distance = tan_alpha * tan_beta / (tan_alpha + tan_beta) * FULL_D_SENSOR;
	double filtered = running_avg(distance);
	// #ifdef LOCALATION_DEBUG
	// // cout << "left_x is " << left_x << " alpha is " << alpha << " in degrees: " << alpha * 180 / PI << endl;
	// // cout << "right_x is " << right_x << " beta is " << beta << " in degrees: " << beta * 180 / PI << endl;
	// cout << "IRRim::calculate_target_coordinate::distance = " << distance << " filtered = " << filtered << endl;;
	// #endif
	return filtered;
}

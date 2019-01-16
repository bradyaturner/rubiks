#include "utils.h"
#include <float.h>
#include <math.h>

#define true 1
#define false 0

// TODO is this necessary? is passing size required?
int indexOf(const int arr[], const int size, const int element) {
	int index = -1;
	for (int i=0; i<size; i++) {
		if (arr[i] == element) {
			index = i;
			break;
		}
	}
	return index;
}

double maxd(double a, double b) {
	return (a>b) ? a : b;
}

double mind(double a, double b) {
	return (a<b) ? a : b;
}

// http://floating-point-gui.de/errors/comparison/
int nearlyEqualF(float a, float b) {
	float absA = fabsf(a);
	float absB = fabsf(b);
	float diff = fabsf(a - b);

	if (a == b) { // shortcut, handles infinities
		return true;
	} else if (a == 0 || b == 0 || diff < FLT_MIN) {
		// a or b is zero or both are extremely close to it
		// relative error is less meaningful here
		return diff < (FLT_EPSILON * FLT_MIN);
	} else { // use relative error
		return diff / fminf((absA + absB), FLT_MAX) < FLT_EPSILON;
	}
}

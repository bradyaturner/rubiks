#ifndef UTILS_H
#define UTILS_H

typedef struct {
	float red;
	float green;
	float blue;
} RGB3f;

int indexOf(const int arr[], const int size, const int element);
double maxd(double a, double b);
double mind(double a, double b);
int nearlyEqualF(float a, float b);

#endif

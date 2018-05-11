#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "vector.h"
#include "quaternion.h"
#include "logger.h"

#define ARRAYSIZE(x) (sizeof(x) / sizeof((x[0])))

#define MAX_ROTATIONS 10

typedef struct {
	Vec3f vector;
	Vec3f rotations[MAX_ROTATIONS];
	Vec3f expected[MAX_ROTATIONS];
} TestCase;

#define NUM_CASES 9

static const TestCase cases[NUM_CASES] =
{
	{
		{1, 0, 0},
		{{0, 90, 0}},
		{{0, 0, 1}}
	},
	{
		{1, 0, 0},
		{{0, -90, 0}},
		{{0, 0, -1}}
	},
	{
		{-1, 0, 0},
		{{0, 90, 0}},
		{{0, 0, -1}}
	},
	{
		{-1, 0, 0},
		{{0, -90, 0}},
		{{0, 0, 1}}
	},
	{
		{0, 1, 0},
		{{90, 0, 0}},
		{{0, 0, -1}}
	},
	{
		{0, 1, 0},
		{{-90, 0, 0}},
		{{0, 0, 1}}
	},
	{
		{0, -1, 0},
		{{90, 0, 0}},
		{{0, 0, 1}}
	},
	{
		{0, -1, 0},
		{{-90, 0, 0}},
		{{0, 0, -1}}
	},
	{
		{1, 0, 0},
		{
			{0, 90, 0},
			{90, 0, 0},
			{0, 0, 90},
		},
		{
			{0, 0, 1},
			{0, 1, 0},
			{1, 0, 0}
		}
	}
};

int test(TestCase c);

int main() {
	int numPassed = 0;
	for(int i=0; i<NUM_CASES; i++) {
		numPassed += test(cases[i]);
	}
	log_info("Test complete. Passed %i out of %i test cases.\n", numPassed, NUM_CASES);
}

int test(TestCase c) {
	log_info("Vector:                           {%f, %f, %f}\n", c.vector.x, c.vector.y, c.vector.z);

	Vec3f result = c.vector;
	int isEqual = 1;
	for (int i=0; i<ARRAYSIZE(c.rotations); i++) {
		if ((c.rotations[i].x + c.rotations[i].y + c.rotations[i].z) == 0)
			continue;
		Quaternion qRot;
		quatInitEuler(&qRot, c.rotations[i]);
		log_info("Applying rotation #%i\n", i);
		log_info("Rotation angles:              {%f, %f, %f}\n", c.rotations[i].x, c.rotations[i].y, c.rotations[i].z);
		log_info("Quaternion from angles:       {%f, %f, %f, %f}\n", qRot.x, qRot.y, qRot.z, qRot.w);
		result = quatVecMultiply(&qRot, result);
		log_info("Vector, rotated:              {%f, %f, %f}\n", result.x, result.y, result.z);
		log_info("Vector, expected:             {%f, %f, %f}\n", c.expected[i].x, c.expected[i].y, c.expected[i].z);
		isEqual &= vec3fCompare(result, c.expected[i]);
	}
	log_info("Calculated result is%s correct.\n\n", isEqual ? "":"n't");
	return isEqual;
}

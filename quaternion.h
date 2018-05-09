#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector.h"

// adapted from https://github.com/IRSmoh/OpenGL_MathDemo/blob/master/src/Quaternion.cpp

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Quaternion;

// util methods
float degToRad(float deg);

// Quaternion methods
void quatInitIdentity(Quaternion *quat);
void quatInitRotations(Quaternion *quat, Vec3f rotations);
void quatInitEuler(Quaternion *quat, Vec3f angles);
void quatInit(Quaternion *quat, float x, float y, float z, float w);
Quaternion quatConjugate(Quaternion *quat);
Quaternion quatInverse(Quaternion *quat);
Quaternion quatNormalize(Quaternion *quat);
Quaternion quatMultiply(Quaternion *left, Quaternion *right);
Quaternion quatVecMultiply(Quaternion *left, Vec3f right);
Quaternion quatMultiplyNoNormal(Quaternion *left, Quaternion *right);
float quatMagnitude(Quaternion *quat);
Quaternion quatFromAxis(Vec3f axis, float angle);
float quatDotProduct(Quaternion *q1, Quaternion *q2);
float* quatToMatrix(Quaternion *quat);
void quatSetEqual(Quaternion *q1, Quaternion *q2);

#endif

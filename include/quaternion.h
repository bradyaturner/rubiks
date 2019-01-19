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

// Quaternion methods
void quat_initIdentity(Quaternion *quat);
void quat_initEuler(Quaternion *quat, Vec3f angles);

Quaternion quat_multiply(Quaternion *left, Quaternion *right);
Vec3f quat_vecMultiply(Quaternion *left, Vec3f right);
Quaternion quat_multiplyNoNormal(Quaternion *left, Quaternion *right);
float* quat_toMatrix(Quaternion *quat);
void quat_setEqual(Quaternion *q1, Quaternion *q2);

#endif

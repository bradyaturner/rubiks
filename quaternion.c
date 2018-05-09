#include "quaternion.h"
#include "vector.h"

#include <math.h>

#define M_PI 3.14159265358979323846264338327

float degToRad(float deg) {
	return deg * M_PI / 180.0;
}

void quatInitIdentity(Quaternion *quat) {
	quat->x = 0;
	quat->y = 0;
	quat->z = 0;
	quat->w = 1;
}

void quatInitRotations(Quaternion *quat, Vec3f rotations) {
	quat->x = rotations.x;
	quat->y = rotations.y;
	quat->z = rotations.z;
	quat->w = 1;
}

void quatInitEuler(Quaternion *quat, Vec3f angles) {
	float heading = degToRad(angles.y);
	float attitude = degToRad(angles.z);
	float bank = degToRad(angles.x);

	float c1 = cos(heading/2);
	float s1 = sin(heading/2);
	float c2 = cos(attitude/2);
	float s2 = sin(attitude/2);
	float c3 = cos(bank/2);
	float s3 = sin(bank/2);
	float c1c2 = c1*c2;
	float s1s2 = s1*s2;
	quat->w = c1c2*c3 - s1s2*s3;
	quat->x = c1c2*s3 + s1s2*c3;
	quat->y = s1*c2*c3 + c1*s2*s3;
	quat->z = c1*s2*c3 - s1*c2*s3;
}

void quatInit(Quaternion *quat, float x, float y, float z, float w) {
	quat->x = x;
	quat->y = y;
	quat->z = z;
	quat->w = w;
}

Quaternion quatConjugate(Quaternion *quat) {
	Quaternion tmp = {-quat->x, -quat->y, -quat->z, -quat->w};
	return tmp;
}

Quaternion quatInverse(Quaternion *quat) {
	return quatConjugate(quat);
}

Quaternion quatNormalize(Quaternion *quat) {
	float mag = quatMagnitude(quat);
	Quaternion result;
	quatInit(&result, quat->x/mag, quat->y/mag, quat->z/mag, quat->w/mag);
	return result;
}

Quaternion quatMultiply(Quaternion *left, Quaternion *right) {
	Quaternion tmp = quatMultiplyNoNormal(left, right);
	return quatNormalize(&tmp);
}

Quaternion quatVecMultiply(Quaternion *left, Vec3f right) {
	Quaternion quatVec = {right.x, right.y, right.z, 0};
	Quaternion inv = quatInverse(left);
	Quaternion rhs = quatMultiplyNoNormal(&quatVec, &inv);
	return quatMultiplyNoNormal(left, &rhs);
}

Quaternion quatMultiplyNoNormal(Quaternion *left, Quaternion *right) {
	float x = left->x*right->w + left->y*right->z - left->z*right->y + left->w*right->x;
	float y = -left->x*right->z + left->y*right->w + left->z*right->x + left->w*right->y;
	float z = left->x*right->y - left->y*right->x + left->z*right->w + left->w*right->z;
	float w = -left->x*right->x - left->y*right->y - left->z*right->z + left->w*right->w;
	Quaternion tmp = {x,y,z,w};
	return tmp;
}

float quatMagnitude(Quaternion *quat) {
	float l = quat->x*quat->x + quat->y*quat->y
		+ quat->z*quat->z + quat->w*quat->w;
	return sqrt(l);
}

Quaternion quatFromAxis(Vec3f axis, float angle) {
	float sinAngle = sin(angle/2);
	Vec3f normalized = vec3fNormalize(axis);
	Quaternion quat;
	quatInit(&quat, normalized.x*sinAngle, normalized.y*sinAngle,
		normalized.z*sinAngle, cos(angle/2));
	return quatNormalize(&quat);
}

float quatDotProduct(Quaternion *left, Quaternion *right) {
	return left->x*right->x + left->y*right->y
		+ left->z*right->z + left->w*right->w;
}

float* quatToMatrix(Quaternion *quat) {
	static float mat[16];
	// initialize identity matrix
	for (int i=0; i<16; i++) {
		mat[i] = (i%5==0) ? 1: 0;
	}

	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
	mat[0] = 1 - 2*quat->y*quat->y - 2*quat->z*quat->z;
	mat[1] = 2*quat->x*quat->y - 2*quat->z*quat->w;
	mat[2] = 2*quat->x*quat->z + 2*quat->y*quat->w;

	mat[4] = 2*quat->x*quat->y + 2*quat->z*quat->w;
	mat[5] = 1 - 2*quat->x*quat->x - 2*quat->z*quat->z;
	mat[6] = 2*quat->y*quat->z - 2*quat->x*quat->w;

	mat[8] = 2*quat->x*quat->z - 2*quat->y*quat->w;
	mat[9] = 2*quat->y*quat->z + 2*quat->x*quat->w;
	mat[10] =  1 - 2*quat->x*quat->x - 2*quat->y*quat->y;

	return mat;
}

void quatSetEqual(Quaternion *q1, Quaternion *q2) {
	q1->x = q2->x;
	q1->y = q2->y;
	q1->z = q2->z;
	q1->w = q2->w;
}

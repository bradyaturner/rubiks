#include "quaternion.h"
#include "vector.h"
#include <float.h>
#include <math.h>
#define M_PI 3.14159265358979323846264338327


// util methods
float degToRad(float deg);

void quat_init(Quaternion *quat, float x, float y, float z, float w);
Quaternion quat_conjugate(Quaternion *quat);
Quaternion quat_inverse(Quaternion *quat);
Quaternion quat_normalize(Quaternion *quat);
float quat_magnitude(Quaternion *quat);
float quat_dotProduct(Quaternion *left, Quaternion *right);

float degToRad(float deg) {
	return deg * M_PI / 180.0;
}

void quat_initIdentity(Quaternion *quat) {
	quat_init(quat, 0, 0, 0, 1);
}

void quat_initEuler(Quaternion *quat, Vec3f angles) {
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
	quat->w = c1c2*c3 + s1s2*s3;
	quat->x = c1c2*s3 - s1s2*c3;
	quat->y = s1*c2*c3 + c1*s2*s3;
	quat->z = c1*s2*c3 - s1*c2*s3;
}

void quat_init(Quaternion *quat, float x, float y, float z, float w) {
	quat->x = x;
	quat->y = y;
	quat->z = z;
	quat->w = w;
}

Quaternion quat_conjugate(Quaternion *quat) {
	Quaternion tmp = {-quat->x, -quat->y, -quat->z, quat->w};
	return tmp;
}

Quaternion quat_inverse(Quaternion *quat) {
	return quat_conjugate(quat);
}

Quaternion quat_normalize(Quaternion *quat) {
	float mag = quat_magnitude(quat);
	Quaternion result;
	quat_init(&result, quat->x/mag, quat->y/mag, quat->z/mag, quat->w/mag);
	return result;
}

Quaternion quat_multiply(Quaternion *left, Quaternion *right) {
	Quaternion tmp = quat_multiplyNoNormal(left, right);
	return quat_normalize(&tmp);
}

// Multiply vector by quaternion
Vec3f quat_vecMultiply(Quaternion *left, Vec3f right) {
	Quaternion quatVec = {right.x, right.y, right.z, 0};
	Quaternion inv = quat_inverse(left);
	Quaternion rhs = quat_multiplyNoNormal(&inv, &quatVec);
	Quaternion tmp = quat_multiplyNoNormal(&rhs, left);
	Vec3f tmp2 = {tmp.x, tmp.y, tmp.z};
	return tmp2;
}

Quaternion quat_multiplyNoNormal(Quaternion *left, Quaternion *right) {
	float x = left->x*right->w + left->y*right->z - left->z*right->y + left->w*right->x;
	float y = -left->x*right->z + left->y*right->w + left->z*right->x + left->w*right->y;
	float z = left->x*right->y - left->y*right->x + left->z*right->w + left->w*right->z;
	float w = -left->x*right->x - left->y*right->y - left->z*right->z + left->w*right->w;
	Quaternion tmp = {x,y,z,w};
	return tmp;
}

float quat_magnitude(Quaternion *quat) {
	float l = quat->x*quat->x + quat->y*quat->y
		+ quat->z*quat->z + quat->w*quat->w;
	return sqrt(l);
}

float* quat_toMatrix(Quaternion *quat) {
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

void quat_setEqual(Quaternion *q1, Quaternion *q2) {
	q1->x = q2->x;
	q1->y = q2->y;
	q1->z = q2->z;
	q1->w = q2->w;
}

int quat_checkEqual(Quaternion *q1, Quaternion *q2) {
	float dot = quat_dotProduct(q1, q2);
	float absdot = fabsf(dot);
	return absdot > 1-FLT_EPSILON;
}

float quat_dotProduct(Quaternion *left, Quaternion *right) {
	return left->x*right->x + left->y*right->y
		+ left->z*right->z + left->w*right->w;
}

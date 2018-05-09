#include "vector.h"
#include <math.h>

Vec3i vectorAdd(Vec3i n1, Vec3i n2) {
	Vec3i tmp;
	tmp.x = n1.x + n2.x;
	tmp.y = n1.y + n2.y;
	tmp.z = n1.z + n2.z;
	return tmp;
}

Vec3i vectorMultiply(Vec3i n1, int c) {
	Vec3i tmp;
	tmp.x = n1.x * c;
	tmp.y = n1.y * c;
	tmp.z = n1.z * c;
	return tmp;
}

Vec3f vec3fNormalize(Vec3f vec) {
	float denom = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	if (denom > 0.000001f) {
		return (Vec3f){vec.x/denom, vec.y/denom, vec.z/denom};
	}
	return vec;
}

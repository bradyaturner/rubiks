#include "vector.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>

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

Vec3f vec3fMultiplyScalar(Vec3f v, float s) {
	Vec3f tmp;
	tmp.x = v.x * s;
	tmp.y = v.y * s;
	tmp.z = v.z * s;
	return tmp;
}

int vec3fCompare(Vec3f a, Vec3f b) {
	return ( nearlyEqualF(a.x,b.x)
		&& nearlyEqualF(a.y,b.y)
		&& nearlyEqualF(a.z, b.z)
	);
}

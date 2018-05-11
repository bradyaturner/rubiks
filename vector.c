#include "vector.h"
#include <math.h>
#include <stdio.h>
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

int vec3fCompare(Vec3f a, Vec3f b) {
/*	if (a.x == b.x) {
		printf("\tx's are equal\n");
	}
	if (a.y == b.y) {
		printf("\ty's are equal\n");
	}
	if (a.z == b.z) {
		printf("\tz's are equal\n");
	} else {
		printf("\t Z NOT EQUAL: %f != %f diff: %f\n", a.z, b.z, a.z-b.z);
	}
*/
	return ( (a.x == b.x || abs(a.x-b.x) <= 0.0000001)
		&& (a.y == b.y || abs(a.y-b.y) <= 0.0000001)
		&& (a.z == b.z || abs(a.z-b.z) <= 0.0000001)
	);
}

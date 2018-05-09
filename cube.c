#include "cube.h"
#include <stdio.h>

void rotateCube(Cube *cube, const Vec3i degrees) {
	printf("Cube quaternion before: %f, %f, %f, %f...\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	printf("Rotating degrees: %i, %i, %i\n", degrees.x, degrees.y, degrees.z);

	Quaternion tmp;
	Vec3f rot = {degrees.x, degrees.y, degrees.z};
	quatInitEuler(&tmp, rot);
	printf("Created temp quaternion from euler angles: %f, %f, %f, %f\n", tmp.x, tmp.y, tmp.z, tmp.w);
	Quaternion result = quatMultiply(&cube->quat, &tmp);
	quatSetEqual(&cube->quat, &result);

	printf("Cube quaternion after: %f, %f, %f, %f...\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
}

void resetRotation(Cube *cube) {
	quatInitIdentity(&cube->quat);
}

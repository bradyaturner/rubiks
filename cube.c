#include "cube.h"
#include <stdio.h>

void rotateCube(Cube *cube, const Vec3i degrees) {
	printf("\n\n[CUBEID: %i] Rotating cube at position: %i\n", cube->id, cube->position);
	printf("[CUBEID: %i] Cube quaternion before: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	printf("[CUBEID: %i] Rotating degrees: %i, %i, %i\n", cube->id, degrees.x, degrees.y, degrees.z);

	Quaternion tmp;
	Vec3f rot = {degrees.x, degrees.y, degrees.z};
	quatInitEuler(&tmp, rot);

	printf("[CUBEID: %i] Created temp quaternion from euler angles: %f, %f, %f, %f\n", cube->id, tmp.x, tmp.y, tmp.z, tmp.w);
	Quaternion result = quatMultiply(&cube->quat, &tmp);
	quatSetEqual(&cube->quat, &result);

	printf("[CUBEID: %i] Cube quaternion after: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
}

void resetRotation(Cube *cube) {
	quatInitIdentity(&cube->quat);
}

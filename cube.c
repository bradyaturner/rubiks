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

float* getColorArray(const Cube cube) {
	static float colors[72];

	for (int i=0; i<NUM_FACES; i++) {
		for (int v=0; v<4; v++) {
			colors[i*12 + v*3] = cube.faces[i]->color.red;
			colors[i*12 + v*3 + 1] = cube.faces[i]->color.green;
			colors[i*12 + v*3 + 2] = cube.faces[i]->color.blue;
		}
	}
	return colors;
}

void initializeCube(Cube *cube, int id, int position) {
		cube->id = id;
		cube->position = position;
		quatInitIdentity(&cube->quat);
		cube->top.color = (RGB3f){1.0, 1.0, 1.0}; // white
		cube->bottom.color = (RGB3f){1.0, 1.0, 0.0}; // yellow
		cube->left.color = (RGB3f){1.0, 0.0, 0.0}; // red
		cube->right.color = (RGB3f){1.0, 0.65, 0.0}; // orange
		cube->front.color = (RGB3f){0.0, 1.0, 0.0}; // green
		cube->back.color = (RGB3f){0.0, 0.0, 1.0}; // blue

		cube->faces[0] = &cube->left;
		cube->faces[1] = &cube->right;
		cube->faces[2] = &cube->bottom;
		cube->faces[3] = &cube->top;
		cube->faces[4] = &cube->front;
		cube->faces[5] = &cube->back;
}

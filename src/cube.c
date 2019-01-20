#include "cube.h"
#include "logger.h"
#include <stdio.h>

static void cube_resetRotation(Cube *cube);
static void cube_resetPosition(Cube *cube);

void cube_rotate(Cube *cube, const Vec3i degrees) {
	log_trace("[CUBEID: %i] Rotating cube at position: %i\n", cube->id, cube->position);
	log_trace("[CUBEID: %i] Cube quaternion before: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	log_trace("[CUBEID: %i] Rotating degrees: %i, %i, %i\n", cube->id, degrees.x, degrees.y, degrees.z);

	Quaternion tmp;
	Vec3f rot = {degrees.x, degrees.y, degrees.z};
	quat_initEuler(&tmp, rot);

	log_trace("[CUBEID: %i] Created temp quaternion from euler angles: %f, %f, %f, %f\n", cube->id, tmp.x, tmp.y, tmp.z, tmp.w);
	Quaternion result = quat_multiply(&cube->quat, &tmp);
	quat_setEqual(&cube->quat, &result);

	log_trace("[CUBEID: %i] Cube quaternion after: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
}

float* cube_getColorArray(const Cube cube) {
	static float colors[72];

	for (int i=0; i<NUM_FACES; i++) {
		for (int v=0; v<4; v++) {
			colors[i*12 + v*3] = cube.faces[i].color.red;
			colors[i*12 + v*3 + 1] = cube.faces[i].color.green;
			colors[i*12 + v*3 + 2] = cube.faces[i].color.blue;
		}
	}
	return colors;
}

void cube_initialize(Cube *cube, int id, int position) {
	cube->id = id;
	cube->position = position;
	cube->initialPosition = position;
	quat_initIdentity(&cube->quat);

	for (int i=0; i<NUM_FACES; i++) {
		cube->faces[i].color = faceData[i].rgb;
	}
}

void cube_reset(Cube *cube) {
	cube_resetPosition(cube);
	cube_resetRotation(cube);
}

void cube_resetPosition(Cube *cube) {
	cube->position = cube->initialPosition;
}
void cube_resetRotation(Cube *cube) {
	quat_initIdentity(&cube->quat);
}

int isCubeInitPos(Cube *cube) {
	return cube->position == cube->initialPosition;
}

int cube_getShownFace(Cube *cube, int face) {
	for(int i=0; i<NUM_FACES; i++) {
		printf("\n\n");
		log_info("cube quat: %f %f %f %f\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
		Vec3f tmp2 = quat_vecMultiply(&cube->quat, faceData[i].normal);

		log_info("Vec for face:%c: before rotate  x:%f y:%f z:%f\n",
			faceData[i].color, faceData[i].normal.x, faceData[i].normal.y, faceData[i].normal.z
		);
		log_info("Vec for face:%c: after rotate   x:%f y:%f z:%f\n", faceData[i].color, tmp2.x, tmp2.y, tmp2.z);
		if (vec3fCompare(tmp2, faceData[face].normal)) {
			log_info("***Cube face %c facing direction %c !\n", faceData[i].color, faceData[face].name);
			return i;
		}
	}
	return -1;
}

int cube_checkPosition(Cube *cube) {
	return (cube->position == cube->initialPosition);
}

int cube_checkRotation(Cube *cube) {
	return quat_checkIdentity(&cube->quat);
}

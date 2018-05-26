#include "cube.h"
#include "logger.h"
#include <stdio.h>

void rotateCube(Cube *cube, const Vec3i degrees) {
	log_trace("[CUBEID: %i] Rotating cube at position: %i\n", cube->id, cube->position);
	log_trace("[CUBEID: %i] Cube quaternion before: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	log_trace("[CUBEID: %i] Rotating degrees: %i, %i, %i\n", cube->id, degrees.x, degrees.y, degrees.z);

	Quaternion tmp;
	Vec3f rot = {degrees.x, degrees.y, degrees.z};
	quatInitEuler(&tmp, rot);

	log_trace("[CUBEID: %i] Created temp quaternion from euler angles: %f, %f, %f, %f\n", cube->id, tmp.x, tmp.y, tmp.z, tmp.w);
	Quaternion result = quatMultiply(&cube->quat, &tmp);
	quatSetEqual(&cube->quat, &result);

	log_trace("[CUBEID: %i] Cube quaternion after: %f, %f, %f, %f...\n", cube->id, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
}

#define NUM_CUBE_VERTICES 4
#define NUM_COLOR_VALUES 4
float* getColorArray(const Cube cube) {

	static float colors[NUM_FACES * NUM_CUBE_VERTICES * NUM_COLOR_VALUES];
	float alpha = 0.0f;

	for (int i=0; i<NUM_FACES; i++) {
		// repeat each color 4 times (4 vertices)
		for (int v=0; v<5; v++) {
			int offset = (i*NUM_CUBE_VERTICES*NUM_COLOR_VALUES) + (v*NUM_COLOR_VALUES);
			colors[offset] = cube.faces[i].color.red;
			colors[offset + 1] = cube.faces[i].color.green;
			colors[offset + 2] = cube.faces[i].color.blue;
			colors[offset + 3] = alpha;
		}
	}
	return colors;
}

float* getNormalArray(const Cube cube) {
	static float normals[72];

	for (int i=0; i<NUM_FACES; i++) {
		for (int v=0; v<4; v++) {
			normals[i*12 + v*3] = faceData[i].normal.x;
			normals[i*12 + v*3 + 1] = faceData[i].normal.y;
			normals[i*12 + v*3 + 2] = faceData[i].normal.z;
		}
	}
	return normals;
}

void initializeCube(Cube *cube, int id, int position) {
	cube->id = id;
	cube->position = position;
	cube->initialPosition = position;
	quatInitIdentity(&cube->quat);

	for (int i=0; i<NUM_FACES; i++) {
		cube->faces[i].color = faceData[i].rgb;
	}
}

void resetCube(Cube *cube) {
	resetPosition(cube);
	resetRotation(cube);
}

void resetPosition(Cube *cube) {
	cube->position = cube->initialPosition;
}
void resetRotation(Cube *cube) {
	quatInitIdentity(&cube->quat);
}

int isCubeInitPos(Cube *cube) {
	return cube->position == cube->initialPosition;
}

int getShownFace(Cube *cube, int face) {
	for(int i=0; i<NUM_FACES; i++) {
		printf("\n\n");
		log_info("cube quat: %f %f %f %f\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
		Vec3f tmp2 = quatVecMultiply(&cube->quat, faceData[i].normal);

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

#include "singlecube.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>

void rotateFace(Cube *cube, int face, int direction) {
	Vec3i deg = layerDegrees[face];
	if (direction <0) {
		deg = vectorMultiply(deg, -1);
	}
	rotateCube(cube, deg);
}

void rotateSingleCube(Cube *cube, int face, int direction) {
	if (face >=0 && face < NUM_FACES) {
		log_info("Rotating cube face %i:%c %sclockwise\n", face, faceNames[face], direction==CLOCKWISE ? "" : "counter");
		rotateFace(cube, face, direction);
	}
	log_info("Cube quaternion: x:%f, y:%f, z:%f, w:%f\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	determineOrientation(cube);
}

void determineOrientation(Cube *cube) {
	for(int i=0; i<NUM_FACES; i++) {
		printf("\n\n");
		log_info("cube quat: %f %f %f %f\n", cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
		Vec3f tmp2 = quatVecMultiply(&cube->quat, faceNormals[i]);

		log_info("Vec for face:%c: before rotate  x:%f y:%f z:%f\n", faceColors[i], faceNormals[i].x, faceNormals[i].y, faceNormals[i].z);
		log_info("Vec for face:%c: after rotate   x:%f y:%f z:%f\n", faceColors[i], tmp2.x, tmp2.y, tmp2.z);
		for (int i2=0; i2<NUM_FACES; i2++) {
//			log_info("***Comparing [%f, %f, %f] : [%f, %f, %f]\n", tmp2.x, tmp2.y, tmp2.z, faceNormals[i2].x, faceNormals[i2].y, faceNormals[i2].z);
			if (vec3fCompare(tmp2, faceNormals[i2])) {
				log_info("***Cube face %c facing direction %c !\n", faceColors[i], faceNames[i2]);
			}
		}
	}
}

void logState(Cube *cube) {
	log_info("%s\n", "CURRENT STATE:");
	for(int i=0; i<NUM_FACES; i++) {
		Vec3f tmp2 = quatVecMultiply(&cube->quat, faceNormals[i]);
		log_info("Vec for face:%c: (unmodified)  x:%f y:%f z:%f\n", faceColors[i], faceNormals[i].x, faceNormals[i].y, faceNormals[i].z);
		log_info("Vec for face:%c: (*quat)       x:%f y:%f z:%f\n\n\n", faceColors[i], tmp2.x, tmp2.y, tmp2.z);
	}
}

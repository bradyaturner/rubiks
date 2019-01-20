#include <math.h>
#include "vector.h"
#include "logger.h"
#include "controller/rubikscontroller.h"

float faceRotationDegrees[NUM_FACES] = {0, 0, 0, 0, 0, 0};
int faceRotationDirection[NUM_FACES] = {0, 0, 0, 0, 0, 0};

Vec3f rc_determineCubeCoord(Cube *cube) {
	// TODO there has to be a smarter way to do this math
	Vec3f coord;

	if (rc_checkCubeInFace(cube, LEFT_FACE)) {
		coord.x = -1;
	} else if (rc_checkCubeInFace(cube, RIGHT_FACE)) {
		coord.x = 1;
	} else {
		coord.x = 0;
	}

	if (rc_checkCubeInFace(cube, DOWN_FACE)) {
		coord.y = -1;
	} else if (rc_checkCubeInFace(cube, UP_FACE)) {
		coord.y = 1;
	} else {
		coord.y = 0;
	}

	if (rc_checkCubeInFace(cube, FRONT_FACE)) {
		coord.z = -1;
	} else if (rc_checkCubeInFace(cube, BACK_FACE)) {
		coord.z = 1;
	} else {
		coord.z = 0;
	}

	return coord;
}

void rc_updateFaceRotations(Rubiks *rubiks, double rotationSpeed) {
	int updatedFaces = 0;
	for (int i=0; i<NUM_FACES; i++) {
		if (faceRotationDirection[i]) {
			updatedFaces++;
			faceRotationDegrees[i] += rotationSpeed * faceRotationDirection[i];
			if (fabsf(faceRotationDegrees[i]) >= 90) {
				rc_rotateFace(rubiks, i, faceRotationDirection[i]);
				faceRotationDegrees[i] = 0;
				faceRotationDirection[i] = 0;
			}
		}
	}
	if (updatedFaces > 1) {
		log_fatal("%s\n", "More than one face rotating at once!");
	}
}

void rc_beginFaceRotation(Rubiks *rubiks, int face, int direction, int instant) {
	log_debug("Begin face rotation: face: %i, direction: %i [instant=%s]\n",
		face, direction, instant ? "yes" : "no"
	);
	if (direction != CLOCKWISE && direction != COUNTERCLOCKWISE) {
		log_error("Invalid direction %i for face %i\n", direction, face);
		return;
	}
	if (face >= NUM_FACES || face < 0) {
		log_error("Invalid face to rotate: %i\n", face);
	}

	printf("%c%s\n", faceData[face].name, direction==1?"":"`");

	if (!rc_isRotating()) {
		if (instant) {
			rc_rotateFace(rubiks, face, direction);
		} else {
			faceRotationDirection[face] = direction;
		}
	}
}

int rc_isRotating() {
	int alreadyRotating = 0;
	for (int i=0; i<NUM_FACES; i++) {
		alreadyRotating += (faceRotationDirection[i]!=0);
	}
	return alreadyRotating;
}

int rc_faceIsRotating(int face) {
	return faceRotationDirection[face] != 0;
}

int rc_getFaceRotationDegrees(int face) {
	return faceRotationDegrees[face];
}

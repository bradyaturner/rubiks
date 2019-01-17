#include <math.h>
#include "logger.h"
#include "view/rubiksview.h"
#include "view/cubeview.h"
#include "view/constants.h"

float faceRotationDegrees[NUM_FACES] = {0, 0, 0, 0, 0, 0};
int faceRotationDirection[NUM_FACES] = {0, 0, 0, 0, 0, 0};
int animationsOn = 1;

Vec3f rc_determineCubeCoord(Cube *cube);
void rc_getFaceRotation(Cube *cube);

void rc_draw(Rubiks *rubiks){
	for(int i=0; i<NUM_CUBES; i++){
		Cube *cube = rc_getCubeAtPos(rubiks, i);
		Vec3f cubeCoord = rc_determineCubeCoord(cube);
		glPushMatrix();
		rc_getFaceRotation(cube);
		cube_draw(*cube, cubeCoord, lineColor, cubeWidth);
		glPopMatrix();
	}
}

Vec3f rc_determineCubeCoord(Cube *cube) {
	// TODO there has to be a smarter way to do this math
	int xPos, yPos, zPos;

	if (rc_checkCubeInFace(cube, LEFT_FACE)) {
		xPos = -1;
	} else if (rc_checkCubeInFace(cube, RIGHT_FACE)) {
		xPos = 1;
	} else {
		xPos = 0;
	}

	if (rc_checkCubeInFace(cube, BOTTOM_FACE)) {
		yPos = -1;
	} else if (rc_checkCubeInFace(cube, TOP_FACE)) {
		yPos = 1;
	} else {
		yPos = 0;
	}

	if (rc_checkCubeInFace(cube, FRONT_FACE)) {
		zPos = -1;
	} else if (rc_checkCubeInFace(cube, BACK_FACE)) {
		zPos = 1;
	} else {
		zPos = 0;
	}

	Vec3f cubeCoord = {xPos*cubeWidth, yPos*cubeWidth, zPos*cubeWidth};
	return cubeCoord;
}

void rc_getFaceRotation(Cube *cube) {
	// assuming that only one face at a time will rotate
	for (int i=0; i<NUM_FACES; i++) {
		if (faceRotationDirection[i] != 0) {
			if (rc_checkCubeInFace(cube, i)) {
				glRotatef( faceRotationDegrees[i], faceData[i].normal.x, faceData[i].normal.y, faceData[i].normal.z );
				break;
			}
		}
	}
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

void rc_beginFaceRotation(Rubiks *rubiks, int face, int direction) {
	log_debug("Begin face rotation: face: %i, direction: %i [animations=%s]\n",
		face, direction, animationsOn ? "on" : "off"
	);
	if (direction != CLOCKWISE && direction != COUNTERCLOCKWISE) {
		log_error("Invalid direction %i for face %i\n", direction, face);
		return;
	}
	if (face >= NUM_FACES || face < 0) {
		log_error("Invalid face to rotate: %i\n", face);
	}

	if (!rc_isRotating()) {
		if (!animationsOn) {
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

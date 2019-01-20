#include "rubiks.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

// Array positions of cubes after a 90-degree clockwise rotation
static const int rotation[FACE_SIZE] = {6, 3, 0, 7, 4, 1, 8, 5, 2}; // to rotate ccw flip face and rotation values

// Cube IDs of cubes in each face of Rubik's Cube
static const int faces[NUM_FACES][FACE_SIZE] =
{
	{0, 3, 6, 9, 12, 15, 18, 21, 24},
	{8, 5, 2, 17, 14, 11, 26, 23, 20},
	{24, 25, 26, 21, 22, 23, 18, 19, 20},
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{6, 7, 8, 15, 16, 17, 24, 25, 26},
	{2, 1, 0, 11, 10, 9, 20, 19, 18}
};

static void rc_translateFace(Rubiks *rubiks, const int face[], const int translation[]);
static int rc_getFace(Rubiks *rubiks, int face, Cube* cubes[]);

void rc_rotateFace(Rubiks *rubiks, int face, int direction) {
	if (face > NUM_FACES || face < 0) {
		log_fatal("Attempted to rotate invalid face #%i\n", face);
		exit(1);
	}
	log_trace("rc_rotateFace(%i): [%i, %i, %i, %i, %i, %i, %i, %i, %i] -> {%i, %i, %i} direction: %sclockwise\n",
		face, faces[face][0], faces[face][1], faces[face][2], faces[face][3], faces[face][4], faces[face][5],
		faces[face][6], faces[face][7], faces[face][8], faceData[face].rotation.x, faceData[face].rotation.y,
		faceData[face].rotation.z, direction==CLOCKWISE ? "":"counter"
	);
	Cube* cubes[FACE_SIZE];
	rc_getFace(rubiks, face, cubes);
	int newPositions[FACE_SIZE];
	for (int i=0; i<9; i++) {
		log_debug("Determining new position for cube at position %i == faces[%i][%i]=%i\n",
			cubes[i]->position,
			face, i, faces[face][i]);

		int index = indexOf(rotation, FACE_SIZE, i);
		newPositions[i] = faces[face][index];
	}

	Vec3i deg = faceData[face].rotation;
	if (direction == CLOCKWISE) {
		rc_translateFace(rubiks, faces[face], newPositions);
	} else {
		rc_translateFace(rubiks, newPositions, faces[face]);
		deg = vectorMultiply(deg, -1);
	}
	for (int i=0; i<FACE_SIZE; i++) {
		cube_rotate(cubes[i], deg);
	}
}

void rc_translateFace(Rubiks *rubiks, const int face[], const int translation[]) {
	Cube* cubes[FACE_SIZE];
	for (int i=0; i<FACE_SIZE; i++) {
		cubes[i] = rc_getCubeAtPos(rubiks, face[i]);
	}

	for (int i=0; i<FACE_SIZE; i++) {
		log_debug("Rotating cube w/ ID=%i at position %i to position %i\n",
			cubes[i]->id,
			cubes[i]->position,
			translation[i]
		);
		cubes[i]->position = translation[i];
	}
}

Cube* rc_getCubeAtPos(Rubiks *rubiks, int cubePosition) {
	Cube* cube = NULL;
	for (int i=0; i<NUM_CUBES; i++) {
		if (rubiks->cubes[i].position == cubePosition) {
			cube = &rubiks->cubes[i];
			break;
		}
	}
	if (cube == NULL) {
		log_fatal("FAILED TO LOCATE CUBE AT POSTITION %i\n", cubePosition);
		exit(1);
	}
	return cube;
}

Cube* rc_getCubeById(Rubiks *rubiks, int cubeId) {
	if (cubeId < 0 || cubeId > NUM_CUBES-1) {
		log_fatal("CUBE ID OUTSIDE RANGE! cubeId=%i\n", cubeId);
	}
	return &rubiks->cubes[cubeId];
}

void rc_initialize(Rubiks *rubiks) {
	for (int i = 0; i<NUM_CUBES; i++) {
		cube_initialize(&rubiks->cubes[i], i, i);
	}
}

void rc_reset(Rubiks *rubiks) {
	for (int i = 0; i<NUM_CUBES; i++) {
		cube_reset(&rubiks->cubes[i]);
	}
}

int rc_getFace(Rubiks *rubiks, int face, Cube* cubes[]) {
	for (int i=0; i<FACE_SIZE; i++) {
		int pos = faces[face][i];
		log_trace("Looking for cube at position = %i", pos);
		cubes[i] = rc_getCubeAtPos(rubiks, pos);
		if (cubes[i] == NULL) {
			log_fatal("No cube found at position %i in face %i\n", pos, face);
			return -1;
		}
		log_trace("Found cube with ID: %i\n", cubes[i]->id);
	}
	return 1;
}

int rc_serialize(Rubiks *rubiks, char* out) {
	char* ptr = out;
	for (int fn=0; fn<NUM_FACES; fn++) {
		rc_getFaceColors(rubiks, fn, ptr);
		ptr += FACE_SIZE;
	}

	for (int i=0; i<FACE_SIZE*NUM_FACES; i++) {
		if (i%3 == 0) {
			printf("\n");
		}
		if (i%9 == 0) {
			printf("\n\n");
		}
		printf("%c", out[i]);
	}
	printf("\n\n");
	return 1;
}

int rc_getFaceColors(Rubiks *rubiks, int face, char* colors) {
	Cube* cubes[FACE_SIZE];
	rc_getFace(rubiks, face, cubes);
	log_debug("Finding cubes for face: %c\n", faceData[face].name);
	for (int i=0; i<FACE_SIZE; i++) {

		// iterate over each cube in face (cubes[0..FACE_SIZE])
		// for each cube in face, iterate over its faces
		int fn = cube_getShownFace(cubes[i], face);
		log_debug("\tFound in face: %c\n", faceData[fn].name);
		if (fn != -1) {
			colors[i] = faceData[fn].color;
		} else {
			log_fatal("Failed to find a visible face for cube: %i\n", cubes[i]->id);
			exit(1);
		}
	}
	return 1;
}

void rc_shuffle(Rubiks *rubiks, int times) {
	for (int i=0; i<times; i++) {
		rc_rotateFace(rubiks, rand()%NUM_FACES, rand()%2==0 ? 1:-1);
	}
}

int rc_checkSolved(Rubiks *rubiks) {
	for (int i=0; i<NUM_CUBES; i++) {
		Cube *cube = &rubiks->cubes[i];
		if (cube->position != cube->initialPosition) {
			return 0;
		}
	}
	return 1;
}

int rc_checkCubeInFace(Cube *cube, int face) {
	return (indexOf(faces[face], FACE_SIZE, cube->position) != -1);
}

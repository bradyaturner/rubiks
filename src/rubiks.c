#include "rubiks.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

void rotateLayer(Rubiks *rubiks, int face, int direction) {
	if (face > NUM_FACES || face < 0) {
		log_fatal("Attempted to rotate invalid face #%i\n", face);
		exit(1);
	}
	log_trace("rotateLayer: [%i, %i, %i, %i, %i, %i, %i, %i, %i] -> {%i, %i, %i} direction: %sclockwise\n",
		layers[face][0], layers[face][1], layers[face][2], layers[face][3], layers[face][4], layers[face][5], layers[face][6], layers[face][7], layers[face][8],
		faceData[face].rotation.x, faceData[face].rotation.y, faceData[face].rotation.z, direction==CLOCKWISE ? "":"counter"
	);
	Cube* cubes[FACE_SIZE];
	getFace(rubiks, face, cubes);
	int newPositions[FACE_SIZE];
	for (int i=0; i<9; i++) {
		log_debug("Determining new position for cube at position %i == layers[%i][%i]=%i\n",
			cubes[i]->position,
			face, i, layers[face][i]);

		int index = indexOf(rotation, FACE_SIZE, i);
		newPositions[i] = layers[face][index];
	}

	Vec3i deg = faceData[face].rotation;
	if (direction == CLOCKWISE) {
		translateLayer(rubiks, layers[face], newPositions);
	} else {
		translateLayer(rubiks, newPositions, layers[face]);
		deg = vectorMultiply(deg, -1);
	}
	for (int i=0; i<FACE_SIZE; i++) {
		rotateCube(cubes[i], deg);
	}
}

void translateLayer(Rubiks *rubiks, const int layer[], const int translation[]) {
	Cube* cubes[FACE_SIZE];
	for (int i=0; i<FACE_SIZE; i++) {
		cubes[i] = findCube(rubiks, layer[i]);
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

void rotateCubeFace(Rubiks *rubiks, int face, int direction) {
	if (face >=0 && face < NUM_FACES) {
		log_debug("Rotating face %i %sclockwise\n", face, direction==CLOCKWISE ? "" : "counter");
		rotateLayer(rubiks, face, direction);
	}
}

Cube* findCube(Rubiks *rubiks, int cubePosition) {
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

void initializeRubiks(Rubiks *rubiks) {
	for (int i = 0; i<NUM_CUBES; i++) {
		initializeCube(&rubiks->cubes[i], i, i);
	}
}

int getFace(Rubiks *rubiks, int face, Cube* cubes[]) {
	for (int i=0; i<FACE_SIZE; i++) {
		int pos = layers[face][i];
		log_trace("Looking for cube at position = %i", pos);
		cubes[i] = findCube(rubiks, pos);
		if (cubes[i] == NULL) {
			log_fatal("No cube found at position %i in face %i\n", pos, face);
			return -1;
		}
		log_trace("Found cube with ID: %i\n", cubes[i]->id);
	}
	return 1;
}

int serializeRubiks(Rubiks *rubiks, char* out) {
	char* ptr = out;
	for (int fn=0; fn<NUM_FACES; fn++) {
		getFaceColors(rubiks, fn, ptr);
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

int getFaceColors(Rubiks *rubiks, int face, char* colors) {
	Cube* cubes[FACE_SIZE];
	getFace(rubiks, face, cubes);
	printf("Finding cubes for face: %c\n", faceData[face].name);
	for (int i=0; i<FACE_SIZE; i++) {

		// iterate over each cube in face (cubes[0..FACE_SIZE])
		// for each cube in face, iterate over its faces
		int fn = getShownFace(cubes[i], face);
		printf("\tFound in face: %c\n", faceData[fn].name);
		if (fn != -1) {
			colors[i] = faceData[fn].color;
		} else {
			log_fatal("Failed to find a visible face for cube: %i\n", cubes[i]->id);
			exit(1);
		}
	}
	return 1;
}

void shuffle(Rubiks *rubiks, int times) {
	for (int i=0; i<times; i++) {
		rotateLayer(rubiks, rand()%NUM_FACES, rand()%2==0 ? 1:-1);
	}
}

void reset(Rubiks *rubiks) {
	for (int i = 0; i<NUM_CUBES; i++) {
		resetCube(&rubiks->cubes[i]);
	}
}

int isRubiksSolved(Rubiks *rubiks) {
	for (int i=0; i<NUM_CUBES; i++) {
		if (!isCubeInitPos(&rubiks->cubes[i])){
			return 0;
		}
	}
	return 1;
}

int cubeInFace(Cube *cube, int face) {
	return (indexOf(layers[face], FACE_SIZE, cube->position) != -1);
}

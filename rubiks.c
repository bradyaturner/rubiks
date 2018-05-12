#include "rubiks.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

void rotateLayer(Rubiks *rubiks, int face, int direction) {
	log_info("rotateLayer: [%i, %i, %i, %i, %i, %i, %i, %i, %i] -> {%i, %i, %i} direction: %sclockwise\n",
		layers[face][0], layers[face][1], layers[face][2], layers[face][3], layers[face][4], layers[face][5], layers[face][6], layers[face][7], layers[face][8],
		layerDegrees[face].x, layerDegrees[face].y, layerDegrees[face].z, direction==CLOCKWISE ? "":"counter"
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

	Vec3i deg = layerDegrees[face];
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
		log_info("Rotating face %i %sclockwise\n", face, direction==CLOCKWISE ? "" : "counter");
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

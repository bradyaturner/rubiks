#include "rubiks.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>

void rotateLayer(Rubiks *rubiks, const int layer[], const Vec3i degrees, int direction) {
	log_info("rotateLayer: [%i, %i, %i, %i, %i, %i, %i, %i, %i] -> {%i, %i, %i} direction: %sclockwise\n",
		layer[0], layer[1], layer[2], layer[3], layer[4], layer[5], layer[6], layer[7], layer[8],
		degrees.x, degrees.y, degrees.z, direction==CLOCKWISE ? "":"counter"
	);
	int indices[9];
	for (int i=0; i<9; i++) {
		log_trace("Looking for cube at position = %i", layer[i]);
		indices[i] = findCube(rubiks, layer[i]);
		log_trace("\tFound cube index: %i\n", indices[i]);
	}
	int newPositions[9];
	for (int i=0; i<9; i++) {
		log_debug("Determining new position for cube at position %i == layer[%i]=%i\n",
			rubiks->cubes[indices[i]].position,
			i, layer[i]);

			int index = indexOf(rotation, 9, i); // the cube in this face position gets rotated to this new face position
			newPositions[i] = layer[index]; // the new position is the cube at that index in the layer (layer values ARE positions)
	}

	Vec3i deg = degrees;
	if (direction == CLOCKWISE) {
		translateLayer(rubiks, layer, newPositions);
	} else {
		translateLayer(rubiks, newPositions, layer);
		deg = vectorMultiply(deg, -1);
	}
	for (int i=0; i<9; i++) {
		rotateCube(&rubiks->cubes[indices[i]], deg);
	}
}

void translateLayer(Rubiks *rubiks, const int layer[], const int translation[]) {
	int indices[9];
	for (int i=0; i<9; i++) {
		indices[i] = findCube(rubiks, layer[i]);
	}

	for (int i=0; i<9; i++) {
		log_debug("Rotating cube w/ ID=%i at position %i to position %i\n",
			rubiks->cubes[indices[i]].id,
			rubiks->cubes[indices[i]].position,
			translation[i]
		);
		rubiks->cubes[indices[i]].position = translation[i];
	}
}

void rotateCubeFace(Rubiks *rubiks, int face, int direction) { // positive anything=cw, negative anything=ccw
	if (face >=0 && face < NUM_FACES) {
		log_info("Rotating face %i %sclockwise\n", face, direction==CLOCKWISE ? "" : "counter");
		rotateLayer(rubiks, layers[face], layerDegrees[face], direction);
	}
}

int findCube(Rubiks *rubiks, int cubePosition) {
	int foundIndex = -1;
	for (int i=0; i<27; i++) {
		if (rubiks->cubes[i].position == cubePosition) {
			foundIndex = i;
			break;
		}
	}
	return foundIndex;
}

void initializeRubiks(Rubiks *rubiks) {
	for (int i = 0; i<27; i++) {
		initializeCube(&rubiks->cubes[i], i, i);
	}
}

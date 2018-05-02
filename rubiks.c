#include "rubiks.h"
#include "utils.h"
#include <stdio.h>

const int rotation[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6}; // to rotate ccw flip layer and rotation values

const int topLayer[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
const Vec3i topLayerDegrees = {0, 90, 0};

const int bottomLayer[9] = {24, 25, 26, 21, 22, 23, 18, 19, 20};
const Vec3i bottomLayerDegrees = {0, -90, 0};

const int frontLayer[9] = {6, 7, 8, 15, 16, 17, 24, 25, 26};
const Vec3i frontLayerDegrees = {0, 0, 90};

const int backLayer[9] = {2, 1, 0, 11, 10, 9, 20, 19, 18};
const Vec3i backLayerDegrees = {0, 0, -90};

const int rightLayer[9] = {8, 5, 2, 17, 14, 11, 26, 23, 20};
const Vec3i rightLayerDegrees = {90, 0, 0};

const int leftLayer[9] = {0, 3, 6, 9, 12, 15, 18, 21, 24};
const Vec3i leftLayerDegrees = {-90, 0, 0};

void rotateLayer(Rubiks *rubiks, const int layer[], const Vec3i degrees, int direction) {
	int indices[9];
	for (int i=0; i<9; i++) {
		printf("Looking for cube at position = %i", layer[i]);
		indices[i] = findCube(rubiks, layer[i]);
		printf("\tFound cube index: %i\n", indices[i]);
	}
	int newPositions[9];
	for (int i=0; i<9; i++) {
		printf("Determining new position for cube at position %i == layer[%i]=%i\n",
			rubiks->cubes[indices[i]].position,
			i, layer[i]);

			int index = indexOf(rotation, 9, i); // the cube in this face position gets rotated to this new face position
			newPositions[i] = layer[index]; // the new position is the cube at that index in the layer (layer values ARE positions)
	}

	Vec3i deg = degrees;
	if (direction >=0) {
		translateLayer(rubiks, layer, newPositions);
	} else {
		translateLayer(rubiks, newPositions, layer);
		deg = vectorMultiply(deg, -1);
	}
	for (int i=0; i<9; i++) {
		rotateCube(&rubiks->cubes[indices[i]], deg);
	}
}

void translateLayer(Rubiks *rubiks, int layer[], int translation[]) {
	for (int i=0; i<9; i++) {
		int index = findCube(rubiks, layer[i]);
		printf("Rotating cube at position %i to position %i\n",
			rubiks->cubes[index].position,
			translation[i]
		);
		rubiks->cubes[index].position = translation[i];
	}
}

void rotateCubeFace(Rubiks *rubiks, int face, int direction) { // positive anything=cw, negative anything=ccw
	switch(face)
	{
		case 1: // top -- rotation about y axis (cw is +deg)
		{
			printf("Rotating top layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, topLayer, topLayerDegrees, direction);
			break;
		}
		case 2: // bottom -- rotation about y axis
			printf("Rotating bottom layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, bottomLayer, bottomLayerDegrees, direction);
			break;
		case 3: // left -- rotation about x axis
			printf("Rotating left layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, leftLayer, leftLayerDegrees, direction);
			break;
		case 4: // right -- rotation about x axis
			printf("Rotating right layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, rightLayer, rightLayerDegrees, direction);
			break;
		case 5: // front
			printf("Rotating front layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, frontLayer, frontLayerDegrees, direction);
			break;
		case 6: // back
			printf("Rotating back layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateLayer(rubiks, backLayer, backLayerDegrees, direction);
			break;
		default:
			break;
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
		rubiks->cubes[i].position = i;
		rubiks->cubes[i].rotation = (Vec3i){0, 0, 0};
		rubiks->cubes[i].top.color = (RGB3f){1.0, 1.0, 1.0}; // white
		rubiks->cubes[i].bottom.color = (RGB3f){1.0, 1.0, 0.0}; // yellow
		rubiks->cubes[i].left.color = (RGB3f){1.0, 0.0, 0.0}; // red
		rubiks->cubes[i].right.color = (RGB3f){1.0, 0.65, 0.0}; // orange
		rubiks->cubes[i].front.color = (RGB3f){0.0, 1.0, 0.0}; // green
		rubiks->cubes[i].back.color = (RGB3f){0.0, 0.0, 1.0}; // blue
	}
}

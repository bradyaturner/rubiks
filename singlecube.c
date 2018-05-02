#include "singlecube.h"
#include "utils.h"
#include <stdio.h>

const Vec3i topLayerDegrees = {0, 90, 0};
const Vec3i bottomLayerDegrees = {0, -90, 0};
const Vec3i frontLayerDegrees = {0, 0, 90};
const Vec3i backLayerDegrees = {0, 0, -90};
const Vec3i rightLayerDegrees = {90, 0, 0};
const Vec3i leftLayerDegrees = {-90, 0, 0};

void rotateVec(Cube *cube, const Vec3i degrees, int direction) {
	Vec3i deg = degrees;
	if (direction <0) {
		deg = vectorMultiply(deg, -1);
	}
	rotateCube(cube, deg);
}

void rotateF(Cube *cube, int face, int direction) { // positive anything=cw, negative anything=ccw
	switch(face)
	{
		case 1: // top -- rotation about y axis (cw is +deg)
		{
			printf("Rotating top layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, topLayerDegrees, direction);
			break;
		}
		case 2: // bottom -- rotation about y axis
			printf("Rotating bottom layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, bottomLayerDegrees, direction);
			break;
		case 3: // left -- rotation about x axis
			printf("Rotating left layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, leftLayerDegrees, direction);
			break;
		case 4: // right -- rotation about x axis
			printf("Rotating right layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, rightLayerDegrees, direction);
			break;
		case 5: // front
			printf("Rotating front layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, frontLayerDegrees, direction);
			break;
		case 6: // back
			printf("Rotating back layer %sclockwise\n", direction>=0 ? "" : "counter");
			rotateVec(cube, backLayerDegrees, direction);
			break;
		default:
			break;
	}
}

void initializeCube(Cube *cube) {
	cube->position = 0;
	cube->rotation = (Vec3i){0, 0, 0};
	cube->top.color = (RGB3f){1.0, 1.0, 1.0}; // white
	cube->bottom.color = (RGB3f){1.0, 1.0, 0.0}; // yellow
	cube->left.color = (RGB3f){1.0, 0.0, 0.0}; // red
	cube->right.color = (RGB3f){1.0, 0.65, 0.0}; // orange
	cube->front.color = (RGB3f){0.0, 1.0, 0.0}; // green
	cube->back.color = (RGB3f){0.0, 0.0, 1.0}; // blue
}

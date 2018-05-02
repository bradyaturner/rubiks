#ifndef CUBE_H
#define CUBE_H

#include "utils.h"
#include "vector.h"

typedef struct {
	RGB3f color;
} CubeFace;

typedef struct {
	CubeFace top;
	CubeFace bottom;
	CubeFace left;
	CubeFace right;
	CubeFace front;
	CubeFace back;
	int position;
	Vec3i rotation; // degrees of rotation about each axis, can be +/- but should be multiples of 90
} Cube;

void rotateCube(Cube *cube, const Vec3i degrees);

#endif

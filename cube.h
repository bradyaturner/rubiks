#ifndef CUBE_H
#define CUBE_H

#include "utils.h"
#include "vector.h"
#include "quaternion.h"

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
	int id;
	int position;
	Quaternion quat;
} Cube;

void rotateCube(Cube *cube, const Vec3i degrees);
void resetRotation(Cube *cube);

#endif

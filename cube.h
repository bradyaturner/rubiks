#ifndef CUBE_H
#define CUBE_H

#include "utils.h"
#include "vector.h"
#include "quaternion.h"

#define NUM_FACES 6

typedef struct cubeFace {
	RGB3f color;
} CubeFace;

typedef struct {
	CubeFace top;
	CubeFace bottom;
	CubeFace left;
	CubeFace right;
	CubeFace front;
	CubeFace back;
	CubeFace* faces[NUM_FACES];
	int id;
	int position;
	Quaternion quat;
} Cube;

void initializeCube(Cube *cube, int id, int position);
void rotateCube(Cube *cube, const Vec3i degrees);
void resetRotation(Cube *cube);
float* getColorArray(const Cube cube);

#endif

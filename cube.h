#ifndef CUBE_H
#define CUBE_H

#include "utils.h"
#include "vector.h"
#include "quaternion.h"

#define NUM_FACES 6
#define LEFT_FACE 0
#define RIGHT_FACE 1
#define BOTTOM_FACE 2
#define TOP_FACE 3
#define FRONT_FACE 4
#define BACK_FACE 5

typedef struct cubeFace {
	RGB3f color;
} CubeFace;

typedef struct {
	CubeFace faces[NUM_FACES];
	int id;
	int position;
	Quaternion quat;
} Cube;

void initializeCube(Cube *cube, int id, int position);
void rotateCube(Cube *cube, const Vec3i degrees);
void resetRotation(Cube *cube);
float* getColorArray(const Cube cube);

#endif

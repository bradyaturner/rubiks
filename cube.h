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

typedef struct {
	RGB3f color;
} CubeFace;

typedef struct {
	char name;
	char color;
	RGB3f rgb;
	Vec3i rotation;
	Vec3f normal;
} CubeFaceData;

typedef struct {
	CubeFace faces[NUM_FACES];
	int id;
	int position;
	int initialPosition;
	Quaternion quat;
} Cube;

static const CubeFaceData faceData[NUM_FACES] = {
	{ 'L', 'R', {1.0, 0.0, 0.0}, {90, 0, 0}, {-1, 0, 0} },
	{ 'R', 'O', {1.0, 0.65, 0.0}, {-90, 0, 0}, {1, 0, 0} },
	{ 'D', 'Y', {1.0, 1.0, 0.0}, {0, 90, 0}, {0, -1, 0} },
	{ 'T', 'W', {1.0, 1.0, 1.0}, {0, -90, 0}, {0, 1, 0} },
	{ 'F', 'B', {0.0, 0.0, 1.0}, {0, 0, 90}, {0, 0, -1} },
	{ 'B', 'G', {0.0, 1.0, 0.0}, {0, 0, -90}, {0, 0, 1} },
};

void initializeCube(Cube *cube, int id, int position);
void resetCube(Cube *cube);
void rotateCube(Cube *cube, const Vec3i degrees);
void resetRotation(Cube *cube);
void resetPosition(Cube *cube);
float* getColorArray(const Cube cube);
int isCubeInitPos(Cube *cube);
int getShownFace(Cube *cube, int face);

#endif

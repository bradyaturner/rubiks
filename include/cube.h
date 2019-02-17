#ifndef CUBE_H
#define CUBE_H

#include "utils.h"
#include "vector.h"
#include "quaternion.h"

#define NUM_FACES 6

#define LEFT_FACE 0
#define RIGHT_FACE 1
#define DOWN_FACE 2
#define UP_FACE 3
#define FRONT_FACE 4
#define BACK_FACE 5

typedef struct {
	RGB3f color;
} CubeFace;

#define LEFT 0
#define ABOVE 1
#define RIGHT 2
#define BELOW 3

typedef struct {
	char name;
	char color;
	RGB3f rgb;
	Vec3i rotation;
	Vec3f normal;
	int neighbors[4];
} CubeFaceData;

static const CubeFaceData faceData[NUM_FACES] = {
	{ 'L', 'R', {1.0, 0.0, 0.0}, {90, 0, 0}, {-1, 0, 0}, {BACK_FACE, UP_FACE, FRONT_FACE, DOWN_FACE} },
	{ 'R', 'O', {1.0, 0.65, 0.0}, {-90, 0, 0}, {1, 0, 0}, {FRONT_FACE, UP_FACE, BACK_FACE, DOWN_FACE} },
	{ 'D', 'Y', {1.0, 1.0, 0.0}, {0, 90, 0}, {0, -1, 0}, {LEFT_FACE, FRONT_FACE, RIGHT_FACE, BACK_FACE} },
	{ 'U', 'W', {1.0, 1.0, 1.0}, {0, -90, 0}, {0, 1, 0}, {LEFT_FACE, BACK_FACE, RIGHT_FACE, FRONT_FACE} },
	{ 'F', 'B', {0.0, 0.0, 1.0}, {0, 0, 90}, {0, 0, -1}, {LEFT_FACE, UP_FACE, RIGHT_FACE, DOWN_FACE} },
	{ 'B', 'G', {0.0, 1.0, 0.0}, {0, 0, -90}, {0, 0, 1}, {RIGHT_FACE, UP_FACE, LEFT_FACE, DOWN_FACE} },
};

typedef struct {
	CubeFace faces[NUM_FACES];
	int id;
	int position;
	int initialPosition;
	Quaternion quat;
} Cube;

void cube_initialize(Cube *cube, int id, int position);
void cube_reset(Cube *cube);
void cube_rotate(Cube *cube, const Vec3i degrees);
float* cube_getColorArray(const Cube cube);
int cube_getShownFace(Cube *cube, int face);
int cube_checkPosition(Cube *cube);
int cube_checkRotation(Cube *cube);

#endif

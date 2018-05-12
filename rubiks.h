#ifndef RUBIKS_H
#define RUBIKS_H

#include "vector.h"
#include "cube.h"

#define CLOCKWISE 1
#define COUNTERCLOCKWISE -1

typedef struct {
	Cube cubes[27];
} Rubiks;

static const int rotation[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6}; // to rotate ccw flip layer and rotation values

static const int layers[6][9] =
{
	{0, 3, 6, 9, 12, 15, 18, 21, 24},
	{8, 5, 2, 17, 14, 11, 26, 23, 20},
	{24, 25, 26, 21, 22, 23, 18, 19, 20},
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{6, 7, 8, 15, 16, 17, 24, 25, 26},
	{2, 1, 0, 11, 10, 9, 20, 19, 18}
};

static const Vec3i layerDegrees[6] =
{
	{90, 0, 0},
	{-90, 0, 0},
	{0, 90, 0},
	{0, -90, 0},
	{0, 0, -90},
	{0, 0, 90}
};

void initializeRubiks(Rubiks *rubiks);
void rotateCubeFace(Rubiks *rubiks, int face, int direction);
Cube* findCube(Rubiks *rubiks, int cubePosition);
void rotateLayer(Rubiks *rubiks, int face, int direction);
void translateLayer(Rubiks *rubiks, const int layer[], const int translation[]);
int getFace(Rubiks *rubiks, int face, Cube* cubes[]);
void shuffle(Rubiks *rubiks, int times);
void reset(Rubiks *rubiks);

#endif

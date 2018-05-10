#ifndef RUBIKS_H
#define RUBIKS_H

#include "vector.h"
#include "cube.h"

typedef struct {
	Cube cubes[27];
} Rubiks;

void initializeRubiks(Rubiks *rubiks);
void rotateCubeFace(Rubiks *rubiks, int face, int direction);
int findCube(Rubiks *rubiks, int cubePosition);
void rotateLayer(Rubiks *rubiks, const int layer[], const Vec3i degrees, int direction);
void translateLayer(Rubiks *rubiks, const int layer[], const int translation[]);

#endif

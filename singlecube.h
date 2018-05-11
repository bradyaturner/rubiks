#ifndef RUBIKS_H
#define RUBIKS_H

#include "vector.h"
#include "cube.h"

#define CLOCKWISE 1
#define COUNTERCLOCKWISE -1

#define FACE_SIZE 1
#define NUM_CUBES 1

void rotateFace(Cube *cube, int face, int direction);
void rotateSingleCube(Cube *cube, int face, int direction);
void determineOrientation(Cube *cube);
void logState(Cube *cube);

#endif

#ifndef RUBIKS_H
#define RUBIKS_H

#include "vector.h"
#include "cube.h"


void initializeCube(Cube *cube);
void rotateF(Cube *cube, int face, int direction);
void rotateVec(Cube *cube, const Vec3i degrees, int direction);

#endif

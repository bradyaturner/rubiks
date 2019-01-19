#ifndef RUBIKSCONTROLLER_H
#define RUBIKSCONTROLLER_H

#include "cube.h"
#include "rubiks.h"

Vec3f rc_determineCubeCoord(Cube *cube);
void rc_updateFaceRotations(Rubiks *rubiks, double rotationSpeed);
void rc_beginFaceRotation(Rubiks *rubiks, int face, int direction, int instant);
int rc_isRotating();
int rc_faceIsRotating(int face);
int rc_getFaceRotationDegrees(int face);

#endif

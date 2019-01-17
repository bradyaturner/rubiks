#ifndef RUBIKSVIEW_H
#define RUBIKSVIEW_H

#include <GL/gl.h>
#include "rubiks.h"

// Keep track of animation of rotated faces
float faceRotationDegrees[NUM_FACES];
int faceRotationDirection[NUM_FACES];
int animationsOn;

void rc_draw(Rubiks *rubiks);
void rc_updateFaceRotations(Rubiks *rubiks, double rotationSpeed);
void rc_beginFaceRotation(Rubiks *rubiks, int face, int direction);
int rc_isRotating();

#endif

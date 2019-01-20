#ifndef RUBIKS_H
#define RUBIKS_H

#include "vector.h"
#include "cube.h"

#define CLOCKWISE 1
#define COUNTERCLOCKWISE -1

#define FACE_SIZE 9
#define NUM_CUBES 27

typedef struct {
	Cube cubes[NUM_CUBES];
} Rubiks;

// Data management
void rc_initialize(Rubiks *rubiks);
void rc_reset(Rubiks *rubiks);
Cube* rc_getCubeAtPos(Rubiks *rubiks, int cubePosition);
Cube* rc_getCubeById(Rubiks *rubiks, int cubeId);

// Control
void rc_rotateFace(Rubiks *rubiks, int face, int direction);
void rc_shuffle(Rubiks *rubiks, int times);

// State checking
int rc_checkSolved(Rubiks *rubiks);
int rc_checkCubeInFace(Cube *cube, int face);

// Serialization methods
int rc_serialize(Rubiks *rubiks, char* out);
int rc_getFaceColors(Rubiks *rubiks, int face, char* colors);

#endif

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

static const int rotation[FACE_SIZE] = {6, 3, 0, 7, 4, 1, 8, 5, 2}; // to rotate ccw flip layer and rotation values

static const int layers[NUM_FACES][FACE_SIZE] =
{
	{0, 3, 6, 9, 12, 15, 18, 21, 24},
	{8, 5, 2, 17, 14, 11, 26, 23, 20},
	{24, 25, 26, 21, 22, 23, 18, 19, 20},
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{6, 7, 8, 15, 16, 17, 24, 25, 26},
	{2, 1, 0, 11, 10, 9, 20, 19, 18}
};

void initializeRubiks(Rubiks *rubiks);
void rotateCubeFace(Rubiks *rubiks, int face, int direction);
Cube* findCube(Rubiks *rubiks, int cubePosition);
void rotateLayer(Rubiks *rubiks, int face, int direction);
void translateLayer(Rubiks *rubiks, const int layer[], const int translation[]);
int getFace(Rubiks *rubiks, int face, Cube* cubes[]);
int serializeRubiks(Rubiks *rubiks, char* out);
int getFaceColors(Rubiks *rubiks, int face, char* colors);
void shuffle(Rubiks *rubiks, int times);
void reset(Rubiks *rubiks);
int isRubiksSolved(Rubiks *rubiks);
int cubeInFace(Cube *cube, int face);

#endif
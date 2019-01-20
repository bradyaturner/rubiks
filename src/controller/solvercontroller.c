#include "controller/solvercontroller.h"
#include "rubiks.h"
#include "cube.h"
#include "logger.h"

#define NUM_STEPS 5

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength);
int checkCurrentState(Rubiks *rubiks);
int checkStep(Rubiks *rubiks, int stepNum);

int checkWhiteCross(Rubiks *rubiks);
int checkWhiteCorners(Rubiks *rubiks);
int checkMiddleLayer(Rubiks *rubiks);
int checkTopFace(Rubiks *rubiks);
int checkFinalLayer(Rubiks *rubiks);

typedef struct{
	const char *name;
	int (*checkFunction)(Rubiks *rubiks);
} Step;

Step steps[NUM_STEPS] = {
	{"WHITE CROSS", &checkWhiteCross},
	{"WHITE CORNERS", &checkWhiteCorners},
	{"MIDDLE LAYER", &checkMiddleLayer},
	{"TOP FACE", &checkTopFace},
	{"FINAL LAYER", &checkFinalLayer}
};

int solver_checkSolved(Rubiks *rubiks) {
	return checkCurrentState(rubiks) == NUM_STEPS;
}

int checkCurrentState(Rubiks *rubiks) {
	int currentStep = 0;
	for ( ; currentStep<NUM_STEPS; currentStep++) {
		if (!checkStep(rubiks, currentStep)) {
			break;
		}
	}
	return currentStep;
}

int checkStep(Rubiks *rubiks, int stepNum) {
	log_info("Checking step: %s\n", steps[stepNum].name);
	int correct = (*steps[stepNum].checkFunction)(rubiks);
	log_info("Step %s is %ssolved\n", steps[stepNum].name, (correct ? "" : "NOT "));
	return correct;
}

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength) {
	int allCorrect = 1;
	for (int i = 0; i < idsLength; i++) {
		Cube *cube = rc_getCubeById(rubiks, ids[i]);
		int correctPos = cube_checkPosition(cube);
		int correctRot = cube_checkRotation(cube);
		log_info("Cube %i correct position: %s, correct rotation: %s\n",
			cube->id, correctPos ? "YES":"NO", correctRot ? "YES":"NO"
		);
		allCorrect = correctPos && correctRot && allCorrect;
	}
	return allCorrect;
}

static int whiteCrossCubeIds[4] = {1, 3, 5, 7};
int checkWhiteCross(Rubiks *rubiks) {
	return checkCubesPosAndRot(rubiks, whiteCrossCubeIds, 4);
}

static int whiteCornersCubeIds[4] = {0, 2, 6, 8};
int checkWhiteCorners(Rubiks *rubiks) {
	return checkCubesPosAndRot(rubiks, whiteCornersCubeIds, 4);
}

static int middleLayerCubeIds[4] = {9, 11, 15, 17};
int checkMiddleLayer(Rubiks* rubiks) {
	return checkCubesPosAndRot(rubiks, middleLayerCubeIds, 4);
}

// only the outside 8 cubes, left pieces can't move and rotation doesn't matter
static int topLayerCubeIds[8] = {18, 19, 20, 21, 23, 24, 25, 26};
int checkTopFace(Rubiks* rubiks) {
	// TODO position and rotation don't matter yet, just matters that yellow is facing up
	return checkCubesPosAndRot(rubiks, topLayerCubeIds, 8); // change this
}

int checkFinalLayer(Rubiks* rubiks) {
	return checkCubesPosAndRot(rubiks, topLayerCubeIds, 8);
}

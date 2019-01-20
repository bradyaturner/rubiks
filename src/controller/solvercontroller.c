#include "controller/solvercontroller.h"
#include "rubiks.h"
#include "cube.h"
#include "logger.h"

#define NUM_STEPS 5

const char *stepNames[NUM_STEPS] = {
	"WHITE CROSS",
	"WHITE CORNERS",
	"MIDDLE LAYER",
	"TOP FACE",
	"FINAL LAYER",
};

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength);
int checkCurrentState(Rubiks *rubiks);
int checkStep(Rubiks *rubiks, int stepNum);

int checkWhiteCrossSolved(Rubiks *rubiks);
int checkWhiteCornersSolved(Rubiks *rubiks);
int checkMiddleLayerSolved(Rubiks *rubiks);
int checkTopFaceSolved(Rubiks *rubiks);
int checkFinalLayerSolved(Rubiks *rubiks);

int checkCubePosition(Cube *cube);
int checkCubeRotation(Cube *cube);

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


// Could reverse the step order and remove 'break's to check all previous steps
int checkStep(Rubiks *rubiks, int stepNum) {
	log_info("Checking step: %s\n", stepNames[stepNum]);
	int correct = 0;
	switch(stepNum) {
		case 0:
			correct = checkWhiteCrossSolved(rubiks);
			break;
		case 1:
			correct = checkWhiteCornersSolved(rubiks);
			break;
		case 2:
			correct = checkMiddleLayerSolved(rubiks);
			break;
		case 3:
			correct = checkTopFaceSolved(rubiks);
			break;
		case 4:
			correct = checkFinalLayerSolved(rubiks);
			break;
	}
	log_info("Step %s is %ssolved\n", stepNames[stepNum], (correct ? "" : "NOT "));
	return correct;
}

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength) {
	int correct = 1;
	for (int i = 0; i < idsLength; i++) {
		Cube *cube = rc_getCubeById(rubiks, ids[i]);
		int cubeSolved = ( checkCubePosition(cube) && checkCubeRotation(cube) );
		log_info("Cube %i %sin correct position and rotation\n", cube->id, cubeSolved ? "" : "NOT ");
		correct = correct && cubeSolved;
	}
	return correct;
}

static int whiteCrossCubeIds[4] = {1, 3, 5, 7};
int checkWhiteCrossSolved(Rubiks *rubiks) {
	return checkCubesPosAndRot(rubiks, whiteCrossCubeIds, 4);
}

static int whiteCornersCubeIds[4] = {0, 2, 6, 8};
int checkWhiteCornersSolved(Rubiks *rubiks) {
	return checkCubesPosAndRot(rubiks, whiteCornersCubeIds, 4);
}

static int middleLayerCubeIds[4] = {9, 11, 15, 17};
int checkMiddleLayerSolved(Rubiks* rubiks) {
	return checkCubesPosAndRot(rubiks, middleLayerCubeIds, 4);
}

// only the outside 8 cubes, left pieces can't move and rotation doesn't matter
static int topLayerCubeIds[8] = {18, 19, 20, 21, 23, 24, 25, 26};
int checkTopFaceSolved(Rubiks* rubiks) {
	// TODO position and rotation don't matter yet, just matters that yellow is facing up
	return checkCubesPosAndRot(rubiks, topLayerCubeIds, 8); // change this
}

int checkFinalLayerSolved(Rubiks* rubiks) {
	return checkCubesPosAndRot(rubiks, topLayerCubeIds, 8);
}

int checkCubePosition(Cube *cube) {
	int correct = (cube->position == cube->initialPosition);
	log_info("Cube %i is %sin correct position\n", cube->id, correct ? "" : "NOT ");
	return correct;
}

static Quaternion notRotated = {
	QUAT_IDENTITY_X, QUAT_IDENTITY_Y,
	QUAT_IDENTITY_Z, QUAT_IDENTITY_W
};
int checkCubeRotation(Cube *cube) {
	int correct = quat_checkEqual(&cube->quat, &notRotated);
	log_info("Cube %i is %sin correct rotation\n", cube->id, correct ? "": "NOT ");
	return correct;
}

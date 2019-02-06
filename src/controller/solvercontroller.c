#include <stdlib.h>

#include "controller/solvercontroller.h"
#include "controller/rubikscontroller.h"
#include "rubiks.h"
#include "cube.h"
#include "logger.h"
#include "stepqueue.h"

#define NUM_STEPS 5

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength);
int checkCurrentState(Rubiks *rubiks);
int checkStep(Rubiks *rubiks, int stepNum);

int checkWhiteCross(Rubiks *rubiks);
int checkWhiteCorners(Rubiks *rubiks);
int checkMiddleLayer(Rubiks *rubiks);
int checkDownFace(Rubiks *rubiks);
int checkFinalLayer(Rubiks *rubiks);

void solveWhiteCross(Rubiks *rubiks);
void solveWhiteCorners(Rubiks *rubiks);
void solveMiddleLayer(Rubiks *rubiks);
void solveDownFace(Rubiks *rubiks);
void solveFinalLayer(Rubiks *rubiks);

typedef struct{
	int num;
	int direction;
} RotAndDir;

RotAndDir shortestDistanceToFace(Rubiks *rubiks, int faceToRotate,
	int startSideFace, int desiredSideFace
);

RotAndDir rotateFaceToTarget(Rubiks *rubiks, int faceToRotate, int fromFace, int toFace);

typedef struct{
	const char *name;
	int (*checkFunction)(Rubiks *rubiks);
	void (*solveFunction)(Rubiks *rubiks);
} StepDefinition;

StepDefinition steps[NUM_STEPS] = {
	{"WHITE CROSS", &checkWhiteCross, &solveWhiteCross},
	{"WHITE CORNERS", &checkWhiteCorners, &solveWhiteCorners},
	{"MIDDLE LAYER", &checkMiddleLayer, &solveMiddleLayer},
	{"DOWN FACE", &checkDownFace, &solveDownFace},
	{"FINAL LAYER", &checkFinalLayer, &solveFinalLayer}
};

StepQueue queue;
void enqueueStep(int faceToRotate, int direction);
void enqueueMultipleStep(int faceToRotate, int direction, int num);

void solver_init() {
	initQueue(&queue);
}

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
	log_debug("Checking step: %s\n", steps[stepNum].name);
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
		log_debug("Cube %i correct position: %s, correct rotation: %s\n",
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

int checkDownFace(Rubiks* rubiks) {
	int correct = 1;
	char faceColor = faceData[DOWN_FACE].color;
	char faceColors[FACE_SIZE+1];
	rc_getFaceColors(rubiks, DOWN_FACE, faceColors);
	for (int i=0; i<FACE_SIZE; i++) {
		correct = correct && (faceColors[i] == faceColor);
	}
	return correct;
}

// only the outside 8 cubes, left pieces can't move and rotation doesn't matter
#define DOWN_FACE_NUM_CUBES 8
static int downFaceCubeIds[DOWN_FACE_NUM_CUBES] = {18, 19, 20, 21, 23, 24, 25, 26};
int checkFinalLayer(Rubiks* rubiks) {
	return checkCubesPosAndRot(rubiks, downFaceCubeIds, DOWN_FACE_NUM_CUBES);
}

void solver_solve(Rubiks *rubiks) {
	if (rc_isRotating()) {
		return;
	}

	if (queue.size == 0) {
		for (int currentStep=0; currentStep<NUM_STEPS; currentStep++) {
			if (!checkStep(rubiks, currentStep)) {
				(*steps[currentStep].solveFunction)(rubiks);
				break;
			}
		}
	}

	if ((queue.size > 0 && !rc_isRotating())) {
		log_info("%s\n", "Next step on queue:");
		Step step = dequeue(&queue);
		rc_beginFaceRotation(rubiks, step.face, step.direction, 0);
	}
}

typedef struct {
	int correctPos;
	int correctRot;
	int stepCubeIndex;
	Cube *cube;
} CubeSolutionState;

CubeSolutionState getNextUnsolved(Rubiks *rubiks, int stepCubes[], int size) {

	int correctPos, correctRot;
	int stepCubeIndex = 0;
	Cube *cube = NULL;

	for ( ; stepCubeIndex<size; stepCubeIndex++) {
		int id = stepCubes[stepCubeIndex];
		cube = rc_getCubeById(rubiks, id);
		correctPos = cube_checkPosition(cube);
		correctRot = cube_checkRotation(cube);
		if (!(correctPos && correctRot)) {
			log_info("Cube %i is unsolved.\n", id);
			break;
		} else {
			log_info("Cube %i is solved.\n", id);
		}
	}
	CubeSolutionState state = {correctPos, correctRot, stepCubeIndex, cube};
	return state;
}

int whiteCrossFaces[4] = {BACK_FACE, LEFT_FACE, RIGHT_FACE, FRONT_FACE};
void solveWhiteCross(Rubiks *rubiks) {
	log_info("%s\n", "Inside solveWhiteCross()");

	CubeSolutionState state = getNextUnsolved(rubiks, whiteCrossCubeIds, 4);

	int currentSideFace = -1;
	for (int faceIndex=0; faceIndex<4; faceIndex++) {
		if (rc_checkCubeInFace(state.cube, whiteCrossFaces[faceIndex])) {
			currentSideFace = whiteCrossFaces[faceIndex];
			break;
		}
	}
	if (currentSideFace == -1) {
		log_fatal("Did not find side face for cube: %i\n", state.cube->id);
		exit(1);
	}

	int currentOtherFace = -1;
	for (int faceIndex=0; faceIndex<NUM_FACES; faceIndex++) {
		if (faceIndex == currentSideFace) {
			continue;
		}
		if (rc_checkCubeInFace(state.cube, faceIndex)) {
			currentOtherFace = faceIndex;
			break;
		}
	}
	if (currentOtherFace == -1) {
		log_fatal("Did not find second face for cube: %i, current side face: %i\n", state.cube->id, currentSideFace);
		exit(1);
	}

	if (state.correctPos && !state.correctRot) {
		log_info("Cube %i is in correct position, but incorrect rotation\n", state.cube->id);
		enqueueStep(currentSideFace, CLOCKWISE);
		enqueueStep(faceData[currentSideFace].neighbors[RIGHT], COUNTERCLOCKWISE);
		enqueueStep(DOWN_FACE, COUNTERCLOCKWISE);
		enqueueStep(faceData[currentSideFace].neighbors[RIGHT], CLOCKWISE);
		// Rotation of side face to top position handled by "in correct face" case
	} else if (rc_checkCubeInFace(state.cube, whiteCrossFaces[state.stepCubeIndex])) {
		log_info("Cube %i is in correct face, but not correct position\n", state.cube->id);
		int startFace = (currentSideFace == whiteCrossFaces[state.stepCubeIndex]) ? currentOtherFace : currentSideFace;
		rotateFaceToTarget(rubiks, whiteCrossFaces[state.stepCubeIndex], startFace, UP_FACE);
	} else if (!state.correctPos) {
		// TODO if in correct face already, rotate to top
		log_info("Cube %i is in incorrect position\n", state.cube->id);
		rotateFaceToTarget(rubiks, currentSideFace, currentOtherFace, DOWN_FACE);
		rotateFaceToTarget(rubiks, DOWN_FACE, currentSideFace, whiteCrossFaces[state.stepCubeIndex]);
		rotateFaceToTarget(rubiks, currentSideFace, DOWN_FACE, currentOtherFace);
		// Rotation to UP_FACE handled by (in correct face) case
	}
}

void solveWhiteCorners(Rubiks *rubiks) {
	log_info("%s\n", "Inside solveWhiteCorners()");

	CubeSolutionState state = getNextUnsolved(rubiks, whiteCornersCubeIds, 4);

	int inUpFace = rc_checkCubeInFace(state.cube, UP_FACE);
	int inDownFace = rc_checkCubeInFace(state.cube, DOWN_FACE);

	log_info("Getting sides for cube %i, pos=%i\n", state.cube->id, state.cube->position);
	// TODO change these to find left face, right face
	int sideFace1 = 0;
	for ( ; sideFace1<NUM_FACES; sideFace1++) {
		if (sideFace1 == UP_FACE || sideFace1 == DOWN_FACE) {
			continue;
		}
		if (rc_checkCubeInFace(state.cube, sideFace1)) {
			break;
		}
	}
	log_info("%s\n", "CHECK HERE");

	int sideFace2 = 0;
	for ( ; sideFace2<NUM_FACES; sideFace2++) {
		if (sideFace2 == UP_FACE || sideFace2 == DOWN_FACE || sideFace2 == sideFace1) {
			continue;
		}
		if (rc_checkCubeInFace(state.cube, sideFace2)) {
			break;
		}
	}
	log_info("%s\n", "CHECK HERE");
	log_info("Cube side faces: %c and %c\n", faceData[sideFace1].name, faceData[sideFace2].name);

	if (!state.correctPos && inUpFace) {
		log_info("Cube %i is in UP_FACE, but not correct position\n", state.cube->id);
		// TODO this can be smarter -- decide which face to rotate based on orientation of cube
		int whiteFace = cube_getShownFace(state.cube, UP_FACE);
		log_info("Cube white face is visible in face: %c\n", faceData[whiteFace].name);
		int faceToRotate = sideFace1;
		int direction = COUNTERCLOCKWISE;

		if (faceData[sideFace1].neighbors[LEFT] == sideFace2) {
			log_info("%s\n", "SF1 to the right");
			if (whiteFace == sideFace2) {
				faceToRotate = sideFace2;
				direction = CLOCKWISE;
			}
		} else {
			log_info("%s\n", "SF2 to the right");
			if (whiteFace != sideFace1) {
				faceToRotate = sideFace2;
			} else {
				direction = CLOCKWISE;
			}
		}
		enqueueStep(faceToRotate, direction);
		enqueueStep(DOWN_FACE, direction);
		enqueueStep(faceToRotate, -direction);
		enqueueStep(DOWN_FACE, -direction);
	} else if ((state.correctPos && !state.correctRot) || (inDownFace && state.cube->position == state.cube->initialPosition + 18)) {
		// in position below, follow algorithm
		log_info("Cube %i located at position below target, pos=%i, target=%i\n",
			state.cube->id, state.cube->position, state.cube->initialPosition
		);
		int faceToRotate = sideFace1;
		int direction = COUNTERCLOCKWISE;

		if (faceData[sideFace1].neighbors[RIGHT] == sideFace2) {
			log_info("%s\n", "SF2 to the right");
			faceToRotate = sideFace2;
		}
		enqueueStep(faceToRotate, direction);
		enqueueStep(DOWN_FACE, direction);
		enqueueStep(faceToRotate, -direction);
		enqueueStep(DOWN_FACE, -direction);
	} else if (inDownFace) {
		log_info("Cube %i located in bottom face, but needs rotated.\n", state.cube->id);
		// TODO determine shortest # rotations and direction
		enqueueStep(DOWN_FACE, CLOCKWISE);
	}
}

void solveMiddleLayer(Rubiks *rubiks) {
}

void solveDownFace(Rubiks *rubiks) {
}

void solveFinalLayer(Rubiks *rubiks) {
}

#define NUM_SIDES 4
RotAndDir shortestDistanceToFace(Rubiks *rubiks, int faceToRotate,
	int startSideFace, int destinationSideFace
) {
	log_info("Looking for shortest distance, rotating face %c from start=%c to dest=%c\n",
		faceData[faceToRotate].name, faceData[startSideFace].name, faceData[destinationSideFace].name
	);
	int startSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, startSideFace);
	int destinationSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, destinationSideFace);
	log_info("start: %i, dest: %i\n", startSideIndex, destinationSideIndex);

	int dist = abs(destinationSideIndex - startSideIndex);
	int direction = startSideIndex < destinationSideIndex ? CLOCKWISE : COUNTERCLOCKWISE;
	direction = (dist < NUM_SIDES/2) ? direction : -direction;
	dist = (dist < NUM_SIDES/2) ? dist : NUM_SIDES-dist;
	RotAndDir ret = {dist, direction};
	return ret;
}

RotAndDir rotateFaceToTarget(Rubiks *rubiks, int faceToRotate, int fromFace, int toFace) {
	RotAndDir rotdir = shortestDistanceToFace(rubiks, faceToRotate, fromFace, toFace);
	log_info("Rotating %c %i times in %i direction to get from %c to %c\n",
		faceData[faceToRotate].name, rotdir.num, rotdir.direction,
		faceData[fromFace].name, faceData[toFace].name
	);
	enqueueMultipleStep(faceToRotate, rotdir.direction, rotdir.num);
	return rotdir;
}

void enqueueStep(int faceToRotate, int direction) {
	Step s = {faceToRotate, direction};
	enqueue(&queue, s);
}

void enqueueMultipleStep(int faceToRotate, int direction, int num) {
	for (int i=0; i<num; i++) {
		enqueueStep(faceToRotate, direction);
	}
}

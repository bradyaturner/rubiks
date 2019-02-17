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
	log_debug("Checking step: %s", steps[stepNum].name);
	int correct = (*steps[stepNum].checkFunction)(rubiks);
	log_debug("Step %s is %ssolved", steps[stepNum].name, (correct ? "" : "NOT "));
	return correct;
}

int checkCubesPosAndRot(Rubiks *rubiks, int *ids, int idsLength) {
	int allCorrect = 1;
	for (int i = 0; i < idsLength; i++) {
		Cube *cube = rc_getCubeById(rubiks, ids[i]);
		int correctPos = cube_checkPosition(cube);
		int correctRot = cube_checkRotation(cube);
		log_debug("Cube %i correct position: %s, correct rotation: %s",
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
		Step step = dequeue(&queue);
		log_info("Next step on queue: %c%s", faceData[step.face].name, (step.direction<0?"'":""));
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
			log_info("Cube %i is unsolved.", id);
			break;
		} else {
			log_debug("Cube %i is solved.", id);
		}
	}
	CubeSolutionState state = {correctPos, correctRot, stepCubeIndex, cube};
	return state;
}

typedef struct {
	int primary;
	int secondary;
} EdgePieceFaces;

int getFaceForCube(Cube *cube, int excludeList[], int excludeListLen) {
	int currentFace = -1;
	for (int faceNum=0; faceNum<NUM_FACES; faceNum++) {
		if (indexOf(excludeList, excludeListLen, faceNum) >=0) {
			continue;
		}
		if (rc_checkCubeInFace(cube, faceNum)) {
			currentFace = faceNum;
			break;
		}
	}
	if (currentFace == -1) {
		log_fatal("Did not find side face for cube: %i", cube->id);
		exit(1);
	}
	return currentFace;
}

EdgePieceFaces getEdgePieceFaces(Cube *cube) {
	// Get two faces that an edge piece resides in
	// left face should be the leftmost face, or the only side face when other is UP/DOWN

	int excludes[] = {UP_FACE, DOWN_FACE};
	int primaryFace = getFaceForCube(cube, excludes, 2);

	excludes[0] = primaryFace;
	int secondaryFace = getFaceForCube(cube, excludes, 1);

	EdgePieceFaces r = {primaryFace, secondaryFace};
	return r;
}

typedef struct {
	int primary;
	int secondary;
	int horizontal;
} CornerPieceFaces;

CornerPieceFaces getCornerPieceFaces(Cube *cube) {
	int excludes[] = {UP_FACE, DOWN_FACE, -1};
	int primary = getFaceForCube(cube, excludes, 2);

	excludes[2] = primary;
	int secondary = getFaceForCube(cube, excludes, 3);

	excludes[0] = primary;
	excludes[1] = secondary;
	int horizontal = getFaceForCube(cube, excludes, 2);

	CornerPieceFaces r = {primary, secondary, horizontal};
	// sort left to right
	if (faceData[secondary].neighbors[RIGHT] == primary) {
		r.primary = secondary;
		r.secondary = primary;
	}
	return r;
}

int whiteCrossFaces[4] = {BACK_FACE, LEFT_FACE, RIGHT_FACE, FRONT_FACE};
void solveWhiteCross(Rubiks *rubiks) {
	log_info("%s", "Inside solveWhiteCross()");

	CubeSolutionState state = getNextUnsolved(rubiks, whiteCrossCubeIds, 4);

	EdgePieceFaces faces = getEdgePieceFaces(state.cube);
	int targetFace = whiteCrossFaces[state.stepCubeIndex];

	if (state.correctPos && !state.correctRot) {
		log_info("Cube %i is in correct position, but incorrect rotation", state.cube->id);
		enqueueStep(faces.primary, CLOCKWISE);
		enqueueStep(faceData[faces.primary].neighbors[RIGHT], COUNTERCLOCKWISE);
		enqueueStep(DOWN_FACE, COUNTERCLOCKWISE);
		enqueueStep(faceData[faces.primary].neighbors[RIGHT], CLOCKWISE);
		// Rotation of side face to top position handled by "in correct face" case
	} else if (rc_checkCubeInFace(state.cube, targetFace)) {
		log_info("Cube %i is in correct face, but not correct position", state.cube->id);
		// TODO rework getEdgePieces so this isn't necessary
		// if primary face is the face we want, rotate away from secondary face
		int startFace = (faces.primary == targetFace) ? faces.secondary : faces.primary;
		rotateFaceToTarget(rubiks, targetFace, startFace, UP_FACE);
	} else if (!state.correctPos) {
		log_info("Cube %i is in incorrect position", state.cube->id);
		rotateFaceToTarget(rubiks, faces.primary, faces.secondary, DOWN_FACE);
		rotateFaceToTarget(rubiks, DOWN_FACE, faces.primary, targetFace);
		rotateFaceToTarget(rubiks, faces.primary, DOWN_FACE, faces.secondary);
		// Rotation to UP_FACE handled by (in correct face) case
	}
}

// moves corner piece from DOWN->UP or UP->DOWN
void repositionCornerPiece(int faceToRotate, int direction) {
	enqueueStep(faceToRotate, direction);
	enqueueStep(DOWN_FACE, direction);
	enqueueStep(faceToRotate, -direction);
	enqueueStep(DOWN_FACE, -direction);
}

void solveWhiteCorners(Rubiks *rubiks) {
	log_info("%s", "Inside solveWhiteCorners()");

	CubeSolutionState state = getNextUnsolved(rubiks, whiteCornersCubeIds, 4);
	CornerPieceFaces faces = getCornerPieceFaces(state.cube);

	// TODO make these rotations smarter -- account for rotation
	if (!state.correctPos && faces.horizontal == UP_FACE) {
		repositionCornerPiece(faces.primary, CLOCKWISE);
	} else if (state.correctPos && !state.correctRot) {
		repositionCornerPiece(faces.primary, CLOCKWISE);
	} else if (state.cube->position == state.cube->initialPosition + 18) {
		repositionCornerPiece(faces.primary, CLOCKWISE);
	} else if (faces.horizontal == DOWN_FACE) {
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
	log_debug("Looking for shortest distance, rotating face %c from start=%c to dest=%c",
		faceData[faceToRotate].name, faceData[startSideFace].name, faceData[destinationSideFace].name
	);
	int startSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, startSideFace);
	int destinationSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, destinationSideFace);
	log_debug("start: %i, dest: %i", startSideIndex, destinationSideIndex);

	int dist = abs(destinationSideIndex - startSideIndex);
	int direction = startSideIndex < destinationSideIndex ? CLOCKWISE : COUNTERCLOCKWISE;
	direction = (dist < NUM_SIDES/2) ? direction : -direction;
	dist = (dist < NUM_SIDES/2) ? dist : NUM_SIDES-dist;
	RotAndDir ret = {dist, direction};
	return ret;
}

RotAndDir rotateFaceToTarget(Rubiks *rubiks, int faceToRotate, int fromFace, int toFace) {
	RotAndDir rotdir = shortestDistanceToFace(rubiks, faceToRotate, fromFace, toFace);
	log_info("Rotating %i%c%s to get from %c to %c",
		rotdir.num, faceData[faceToRotate].name, rotdir.direction<0?"'":"",
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

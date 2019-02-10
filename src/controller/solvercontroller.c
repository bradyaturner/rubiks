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

typedef struct {
	int primary;
	int secondary;
} EdgePieceFaces;

typedef struct {
	int primary;
	int secondary;
	int horizontal;
} CornerPieceFaces;

RotAndDir shortestDistanceToFace(int faceToRotate, int startSideFace, int desiredSideFace);
RotAndDir rotateFaceToTarget(int faceToRotate, int fromFace, int toFace);
EdgePieceFaces getEdgePieceFaces(Cube *cube);
CornerPieceFaces getCornerPieceFaces(Cube *cube);

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
static EdgePieceFaces middleLayerFaces[4] = {
	{BACK_FACE, LEFT_FACE},
	{RIGHT_FACE, BACK_FACE},
	{LEFT_FACE, FRONT_FACE},
	{FRONT_FACE, RIGHT_FACE}
};

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
		log_info("%s", "Queue empty, generating next steps");
		rc_serializeState(rubiks);
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
	rubiks->cubeInProgress = cube->id;
	return state;
}

CubeSolutionState getNextUnsolvedInFace(Rubiks *rubiks, int stepCubes[], int size, int face) {

	int correctPos, correctRot;
	int stepCubeIndex = 0;
	Cube *cube = NULL;
	int found = 0;

	for ( ; stepCubeIndex<size; stepCubeIndex++) {
		int id = stepCubes[stepCubeIndex];
		cube = rc_getCubeById(rubiks, id);
		correctPos = cube_checkPosition(cube);
		correctRot = cube_checkRotation(cube);
		if (!(correctPos && correctRot)) {
			log_info("Cube %i is unsolved.", id);
			EdgePieceFaces faces = getEdgePieceFaces(cube);
			if (faces.secondary == face) {
				found = 1;
				break;
			} else {
				log_info("Cube %i is unsolved, but not in face:%c", cube->id, faceData[face].name);
			}
		} else {
			log_debug("Cube %i is solved.", id);
		}
	}

	CubeSolutionState state;
	if (!found) {
		state = (CubeSolutionState){0, 0, -1, NULL};
	} else {
		rubiks->cubeInProgress = cube->id;
		state = (CubeSolutionState){correctPos, correctRot, stepCubeIndex, cube};
	}

	return state;
}

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
	int primary = getFaceForCube(cube, excludes, 2);

	excludes[0] = primary;
	int secondary = getFaceForCube(cube, excludes, 1);

	EdgePieceFaces r = {primary, secondary};

	// sort left to right
	if ((secondary != DOWN_FACE) && (secondary != UP_FACE) && (faceData[secondary].neighbors[RIGHT] == primary)) {
		r.primary = secondary;
		r.secondary = primary;
	}
	return r;
}

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
		rotateFaceToTarget(targetFace, startFace, UP_FACE);
	} else if (!state.correctPos) {
		log_info("Cube %i is in incorrect position", state.cube->id);
		rotateFaceToTarget(faces.primary, faces.secondary, DOWN_FACE);
		rotateFaceToTarget(DOWN_FACE, faces.primary, targetFace);
		rotateFaceToTarget(faces.primary, DOWN_FACE, faces.secondary);
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

typedef enum {DownToLeft, DownToRight, Middle, IncorrectSide, MLSolved, MLUnknown} MiddleLayerForm;
void middleLayerRotationSequence(MiddleLayerForm form, int face1, int face2) {
	if (form == MLSolved) {
		log_error("%s", "Attempting to solve already solved step!");
		exit(1);
	}
	int direction = form ? COUNTERCLOCKWISE : CLOCKWISE;
	enqueueStep(DOWN_FACE, direction);
	enqueueStep(face2, direction);
	enqueueStep(DOWN_FACE, -1*direction);
	enqueueStep(face2, -1*direction);
	enqueueStep(DOWN_FACE, -1*direction);
	enqueueStep(face1, -1*direction);
	enqueueStep(DOWN_FACE, direction);
	enqueueStep(face1, direction);
}

void solveMiddleLayer(Rubiks *rubiks) {
	log_info("%s", "Inside solveMiddleLayer()");

	CubeSolutionState state = getNextUnsolvedInFace(rubiks, middleLayerCubeIds, 4, DOWN_FACE);
	if (state.cube == NULL) {
		log_warn("%s", "Cube is NULL, need to find a different one");
		state = getNextUnsolved(rubiks, middleLayerCubeIds, 4);
	}
	log_info("Attempting to solve edge piece %i for middle layer", state.cube->id);

	EdgePieceFaces faces = getEdgePieceFaces(state.cube);
	EdgePieceFaces target = middleLayerFaces[state.stepCubeIndex];

	MiddleLayerForm form = MLUnknown;
	int faceToRotate;

	if (faces.secondary == DOWN_FACE) {
		int shownFace = cube_getShownFace(state.cube, faces.primary);
		int shownFaceSecondary = cube_getShownFace(state.cube, faces.secondary);
		if ((shownFace == faces.primary) && (shownFaceSecondary == target.primary)) {
			faceToRotate = faceData[faces.primary].neighbors[LEFT];
			form = DownToLeft;
		} else if (shownFace == faces.primary){
			faceToRotate = faceData[faces.primary].neighbors[RIGHT];
			form = DownToRight;
		} else {
			faceToRotate = shownFace;
			form = IncorrectSide;
		}
	} else if (faces.secondary == faceData[target.primary].neighbors[RIGHT]) {
		faceToRotate = faceData[faces.primary].neighbors[RIGHT];
		form = Middle;
	} else if (faces.secondary == target.primary){
		faceToRotate = faceData[faces.primary].neighbors[RIGHT];
		form = DownToRight; // rotate cube out of right pos
	}

	if (form == MLUnknown) {
		log_fatal("%s", "UNEXPECTED STATE");
		exit(1);
	} else if (form == IncorrectSide) {
		rotateFaceToTarget(DOWN_FACE, faces.primary, faceToRotate);
	} else {
		middleLayerRotationSequence(form, faces.primary, faceToRotate);
	}
}

typedef enum {Line, Center, LShape, YCrSolved, Unknown} YCrossForm;
void yellowCrossRotationSequence(YCrossForm form) {
	if (form == YCrSolved) {
		log_error("%s", "Attempting to solve already solved step!");
	}
	enqueueStep(BACK_FACE, CLOCKWISE);
	enqueueStep(form ? DOWN_FACE : RIGHT_FACE, CLOCKWISE);
	enqueueStep(form ? RIGHT_FACE : DOWN_FACE, CLOCKWISE);
	enqueueStep(form ? DOWN_FACE : RIGHT_FACE, COUNTERCLOCKWISE);
	enqueueStep(form ? RIGHT_FACE : DOWN_FACE, COUNTERCLOCKWISE);
	enqueueStep(BACK_FACE, COUNTERCLOCKWISE);
}

static int ycCubePositions[4] = {19, 23, 25, 21}; // in clockwise order
//static int yellowCornersCubeIds[4] = {18, 20, 24, 26};
void solveDownFace(Rubiks *rubiks) {
	// TWO STEPS:
	// 1) Solve yellow cross
	// 2) Solve yellow corners
	// Don't care about colors on sides yet

	log_info("%s", "Inside solveDownFace");
	// Solve yellow cross
	int numSolved = 0;
	int cubesSolved[4] = {0, 0, 0, 0};
	for (int index=0; index<4; index++) {
		Cube *cube = rc_getCubeAtPos(rubiks, ycCubePositions[index]);
		log_info("Cube %i at pos %i", cube->id, cube->position);
		int face = cube_getShownFace(cube, DOWN_FACE);
		log_info("Cube %i color shown in DOWN_FACE: %c", cube->id, faceData[face].color);
		if (face == DOWN_FACE) {
			numSolved++;
			cubesSolved[index] = 1;
		}
	}
	log_info("Number of yellow cross pieces in correct orientation: %i", numSolved);

	YCrossForm form = Unknown;
	if (numSolved == 4) {
		log_info("%s", "Cross is solved");
		form = YCrSolved;
	} else if (numSolved == 0) {
		log_info("%s", "No cubes in cross are solved");
		form = Center;
	} else if (cubesSolved[0] && cubesSolved[2]) {
		log_info("Straight line of cross cubes is solved: %i, %i", ycCubePositions[0], ycCubePositions[2]);
		enqueueStep(DOWN_FACE, CLOCKWISE); // rotate to match pattern
		form = Line;
	} else if (cubesSolved[1] && cubesSolved[3]) {
		log_info("Straight line of cross cubes is solved: %i, %i", ycCubePositions[1], ycCubePositions[3]);
		form = Line;
	} else if (numSolved == 2) {
		int firstCube = 0;
		int secondCube = -1;
		for ( ; firstCube<4; firstCube++) {
			if (cubesSolved[firstCube] && cubesSolved[(firstCube+1)%4]) {
				secondCube = (firstCube+1)%4;
				break;
			}
		}
		if (secondCube >= 0) {
			log_info("Cube L shape formed with cubes at positions: %i, %i",
				ycCubePositions[firstCube], ycCubePositions[secondCube]
			);
			Cube *cube = rc_getCubeAtPos(rubiks, ycCubePositions[secondCube]);
			EdgePieceFaces faces = getEdgePieceFaces(cube);
			rotateFaceToTarget(DOWN_FACE, faces.primary, LEFT_FACE); // rotate to match pattern
			form = LShape;
		}
	}
	if (form == Unknown) {
		log_fatal("%s", "Unable to determine yellow cross form!");
		exit(1);
	} else if (form != YCrSolved) {
		yellowCrossRotationSequence(form);
	}

	// TODO solve corners
}

void solveFinalLayer(Rubiks *rubiks) {
}

#define NUM_SIDES 4
RotAndDir shortestDistanceToFace(int faceToRotate, int startSideFace, int destinationSideFace) {
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

RotAndDir rotateFaceToTarget(int faceToRotate, int fromFace, int toFace) {
	RotAndDir rotdir = shortestDistanceToFace(faceToRotate, fromFace, toFace);
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

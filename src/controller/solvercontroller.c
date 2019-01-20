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
int checkDownFace(Rubiks *rubiks);
int checkFinalLayer(Rubiks *rubiks);

void solveWhiteCross(Rubiks *rubiks);
void solveWhiteCorners(Rubiks *rubiks);
void solveMiddleLayer(Rubiks *rubiks);
void solveDownFace(Rubiks *rubiks);
void solveFinalLayer(Rubiks *rubiks);

typedef struct{
	const char *name;
	int (*checkFunction)(Rubiks *rubiks);
	void (*solveFunction)(Rubiks *rubiks);
} Step;

Step steps[NUM_STEPS] = {
	{"WHITE CROSS", &checkWhiteCross, &solveWhiteCross},
	{"WHITE CORNERS", &checkWhiteCorners, &solveWhiteCorners},
	{"MIDDLE LAYER", &checkMiddleLayer, &solveMiddleLayer},
	{"DOWN FACE", &checkDownFace, &solveDownFace},
	{"FINAL LAYER", &checkFinalLayer, &solveFinalLayer}
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
	for (int currentStep=0; currentStep<NUM_STEPS; currentStep++) {
		if (!checkStep(rubiks, currentStep)) {
			(*steps[currentStep].solveFunction)(rubiks);
			return;
		}
	}
}

int whiteCrossFaces[4] = {BACK_FACE, LEFT_FACE, RIGHT_FACE, FRONT_FACE};
void solveWhiteCross(Rubiks *rubiks) {
	// TODO find white edge pieces
	// rotate into position
	// verify no other solved pieces were moved
	// use display rotation if animationOn == true
	log_info("%s\n", "Inside solveWhiteCross()");

	for (int i=0; i<4; i++) {
		int id = whiteCrossCubeIds[i];
		Cube *cube = rc_getCubeById(rubiks, id);
		int correctPos = cube_checkPosition(cube);
		int correctRot = cube_checkRotation(cube);
		int correctSideFace = rc_checkCubeInFace(cube, whiteCrossFaces[i]); // TODO change for each cube
		int inUpFace = rc_checkCubeInFace(cube, UP_FACE);
		int inDownFace = rc_checkCubeInFace(cube, DOWN_FACE);

		log_info("Cube %i correct position: %s, correct rotation: %s\n",
			cube->id, correctPos ? "YES":"NO", correctRot ? "YES":"NO"
		);
		log_info("Cube %i -- in correct side face? %s, in top face? %s\n",
			cube->id, correctSideFace ? "YES":"NO", inUpFace ? "YES":"NO"
		);
		if (correctPos && correctRot) {
			continue;
		} else if (!correctPos && correctSideFace) {
			//for (int rotations=0; rotations<3; rotations++) {
				rc_rotateFace(rubiks, whiteCrossFaces[i], CLOCKWISE);
			//	correctPos = cube_checkPosition(cube);
			//	if (correctPos) {
			//		break;
			//	}
			//}
		/*} else if (!correctPos && inUpFace) {
			//for (int rotations=0; rotations<3; rotations++) {
			// THIS IS WRONG!! this will undo other faces
			// Should rotate it to bottom
				rc_rotateFace(rubiks, UP_FACE, CLOCKWISE);
			//	correctPos = cube_checkPosition(cube);
			//	if (correctPos) {
			//		break;
			//	}
			//} */
		} else if (inDownFace && !correctSideFace) {
			//for (int rotations=0; rotations<3; rotations++) {
				rc_rotateFace(rubiks, DOWN_FACE, CLOCKWISE);
			//	correctSideFace = rc_checkCubeInFace(cube, whiteCrossFaces[i]);
			//	if (correctSideFace) {
			//		break;
			//	}
			//}
		} else if (!correctPos && !correctRot) {
			log_info("Cube %i in incorrect position, incorrect rotation", cube->id);
			//rotate the face it is in...
			int currentFace = -1;
			// locate cube in a side face
			for (int faceIndex=0; faceIndex<4; faceIndex++) {
				if (rc_checkCubeInFace(cube, whiteCrossFaces[faceIndex])) {
					currentFace = whiteCrossFaces[faceIndex];
					break;
				}
			}
			// TODO rotating a cube down could un-solve another cube
			// need to rotate face back after rotating bottom away
			// queue up moves?
			int numRotations;
			for (numRotations=0; numRotations<3; numRotations++) {
				if (rc_checkCubeInFace(cube, DOWN_FACE)) {
					break;
				}
				rc_rotateFace(rubiks, currentFace, CLOCKWISE);
			}
			rc_rotateFace(rubiks, DOWN_FACE, CLOCKWISE);
			// put rotated face back where it started
			for ( ; numRotations>=0; numRotations--) {
				rc_rotateFace(rubiks, currentFace, CLOCKWISE);
			}
		} else if (correctPos && !correctRot) {
			log_info("Cube %i in correct position but rotated wrong...\n", cube->id);
			rc_rotateFace(rubiks, whiteCrossFaces[i], COUNTERCLOCKWISE);
			rc_rotateFace(rubiks, UP_FACE, CLOCKWISE);
			rc_rotateFace(rubiks, faceData[whiteCrossFaces[i]].faceToLeft, COUNTERCLOCKWISE);
			rc_rotateFace(rubiks, UP_FACE, COUNTERCLOCKWISE);
		}
		return;
	}
}

void solveWhiteCorners(Rubiks *rubiks) {
}

void solveMiddleLayer(Rubiks *rubiks) {
}

void solveDownFace(Rubiks *rubiks) {
}

void solveFinalLayer(Rubiks *rubiks) {
}

int shortestDirectionToFace(Rubiks *rubiks, int faceToRotate, int startSideFace, int desiredSideFace) {
	// TODO return CLOCKWISE or COUNTERCLOCKWISE
	// depending on which direction gives a shorter path to the desiredSideFace
	// ex, rotating fontFace to make leftFace cubes on downFace -- 1 CCW, or 3 CW -- return CCW
	return CLOCKWISE;
}

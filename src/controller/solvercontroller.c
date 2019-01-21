#include <stdlib.h>

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

int shortestDirectionToFace(Rubiks *rubiks, int faceToRotate,
	int startSideFace, int desiredSideFace
);

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
		int correctSideFace = rc_checkCubeInFace(cube, whiteCrossFaces[i]);
		int inUpFace = rc_checkCubeInFace(cube, UP_FACE);
		int inDownFace = rc_checkCubeInFace(cube, DOWN_FACE);

		int currentSideFace = -1;
		// locate cube in a side face
		for (int faceIndex=0; faceIndex<4; faceIndex++) {
			if (rc_checkCubeInFace(cube, whiteCrossFaces[faceIndex])) {
				currentSideFace = whiteCrossFaces[faceIndex];
				break;
			}
		}

		// other face that cube exists in, in addition to side face
		int startFace = 0;
		for ( ; startFace<NUM_FACES; startFace++) {
			if (startFace == currentSideFace) {
				continue;
			}
			if (rc_checkCubeInFace(cube, startFace)) {
				break;
			}
		}

		log_info("Cube %i correct position: %s, correct rotation: %s\n",
			cube->id, correctPos ? "YES":"NO", correctRot ? "YES":"NO"
		);
		log_info("Cube %i -- in correct side face? %s, in top face? %s\n",
			cube->id, correctSideFace ? "YES":"NO", inUpFace ? "YES":"NO"
		);
		if (correctPos && correctRot) {
			continue;
		} else if (!correctPos && correctSideFace) {
			// when in correct face, want to use adjacent face as starting location
			if (startFace == whiteCrossFaces[i]) {
				startFace = currentSideFace;
			}
			int direction = shortestDirectionToFace(rubiks, whiteCrossFaces[i], startFace, UP_FACE);
			rc_rotateFace(rubiks, whiteCrossFaces[i], direction);
		} else if (inDownFace && !correctSideFace) {
			int direction = shortestDirectionToFace(rubiks, DOWN_FACE, currentSideFace, whiteCrossFaces[i]);
			rc_rotateFace(rubiks, DOWN_FACE, direction);
		} else if (!correctPos && !correctRot) {
			log_info("Cube %i in incorrect position, incorrect rotation\n", cube->id);
			log_info("Start location: side: %i, startFace: %i\n", currentSideFace, startFace);
			int direction = shortestDirectionToFace(rubiks, startFace, currentSideFace, DOWN_FACE);
			int numRotations;
			for (numRotations=0; numRotations<3; numRotations++) {
				if (rc_checkCubeInFace(cube, DOWN_FACE)) {
					break;
				}
				rc_rotateFace(rubiks, currentSideFace, direction);
			}
			int downFaceDirection = shortestDirectionToFace(rubiks, DOWN_FACE, currentSideFace, whiteCrossFaces[i]);
			rc_rotateFace(rubiks, DOWN_FACE, downFaceDirection);
			// put rotated face back where it started
			for ( ; numRotations>0; numRotations--) {
				rc_rotateFace(rubiks, currentSideFace, -1*direction);
			}
		} else if (correctPos && !correctRot) {
			log_info("Cube %i in correct position but rotated wrong...\n", cube->id);
			rc_rotateFace(rubiks, whiteCrossFaces[i], COUNTERCLOCKWISE);
			rc_rotateFace(rubiks, UP_FACE, CLOCKWISE);
			rc_rotateFace(rubiks, faceData[whiteCrossFaces[i]].neighbors[LEFT], COUNTERCLOCKWISE);
			rc_rotateFace(rubiks, UP_FACE, COUNTERCLOCKWISE);
		}
		return;
	}
}

void solveWhiteCorners(Rubiks *rubiks) {
	log_info("%s\n", "Inside solveWhiteCorners()");

	for (int i=0; i<4; i++) {
		int id = whiteCornersCubeIds[i];
		Cube *cube = rc_getCubeById(rubiks, id);
		int correctPos = cube_checkPosition(cube);
		int correctRot = cube_checkRotation(cube);
		//int correctSideFace = rc_checkCubeInFace(cube, whiteCrossFaces[i]);
		int inUpFace = rc_checkCubeInFace(cube, UP_FACE);
		int inDownFace = rc_checkCubeInFace(cube, DOWN_FACE);

		log_info("Getting sides for cube %i, pos=%i\n", cube->id, cube->position);
		int sideFace1 = 0;
		for ( ; sideFace1<NUM_FACES; sideFace1++) {
			if (sideFace1 == UP_FACE || sideFace1 == DOWN_FACE) {
				continue;
			}
			if (rc_checkCubeInFace(cube, sideFace1)) {
				break;
			}
		}
		log_info("%s\n", "CHECK HERE");

		int sideFace2 = 0;
		for ( ; sideFace2<NUM_FACES; sideFace2++) {
			if (sideFace2 == UP_FACE || sideFace2 == DOWN_FACE || sideFace2 == sideFace1) {
				continue;
			}
			if (rc_checkCubeInFace(cube, sideFace2)) {
				break;
			}
		}
		log_info("%s\n", "CHECK HERE");
		log_info("Cube side faces: %c and %c\n", faceData[sideFace1].name, faceData[sideFace2].name);

		if (correctPos && correctRot) {
			continue;
		} else if (!correctPos && inUpFace) {
			log_info("Cube %i is in UP_FACE, but not correct position\n", cube->id);
			// TODO this can be smarter -- decide which face to rotate based on orientation of cube
			int whiteFace = cube_getShownFace(cube, UP_FACE);
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
			rc_rotateFace(rubiks, faceToRotate, direction);
			rc_rotateFace(rubiks, DOWN_FACE, direction);
			rc_rotateFace(rubiks, faceToRotate, -direction);
			rc_rotateFace(rubiks, DOWN_FACE, -direction);
		} else if ((correctPos && !correctRot) || (inDownFace && cube->position == cube->initialPosition + 18)) {
			// in position below, follow algorithm
			log_info("Cube %i located at position below target, pos=%i, target=%i\n",
				cube->id, cube->position, cube->initialPosition
			);
			int faceToRotate = sideFace1;
			int direction = COUNTERCLOCKWISE;

			if (faceData[sideFace1].neighbors[RIGHT] == sideFace2) {
				log_info("%s\n", "SF2 to the right");
				faceToRotate = sideFace2;
			}
			for (int numIterations=0; numIterations<4; numIterations++) {
				rc_rotateFace(rubiks, faceToRotate, direction);
				rc_rotateFace(rubiks, DOWN_FACE, direction);
				rc_rotateFace(rubiks, faceToRotate, -direction);
				rc_rotateFace(rubiks, DOWN_FACE, -direction);
				correctPos = cube_checkPosition(cube);
				correctRot = cube_checkRotation(cube);
				if (correctPos && correctRot) {
					break;
				}
			}
		} else if (inDownFace) {
			log_info("Cube %i located in bottom face, but needs rotated.\n", cube->id);
			for (int i=0; i<3; i++) {
				// TODO determine shortest # rotations and direction
				rc_rotateFace(rubiks, DOWN_FACE, CLOCKWISE);
				if (cube->position == cube->initialPosition+18) {
					break;
				}
			}
		}
		return; // should only reach here if a cube isn't solved -- don't modify further cubes until this one is done
	}
}

void solveMiddleLayer(Rubiks *rubiks) {
}

void solveDownFace(Rubiks *rubiks) {
}

void solveFinalLayer(Rubiks *rubiks) {
}

#define NUM_SIDES 4
int shortestDirectionToFace(Rubiks *rubiks, int faceToRotate,
	int startSideFace, int destinationSideFace
) {
	log_info("Looking for shortest direction, rotating face %c from start=%c to dest=%c\n",
		faceData[faceToRotate].name, faceData[startSideFace].name, faceData[destinationSideFace].name
	);
	int startSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, startSideFace);
	int destinationSideIndex = indexOf(faceData[faceToRotate].neighbors, 4, destinationSideFace);
	log_info("start: %i, dest: %i\n", startSideIndex, destinationSideIndex);

	int dist = abs(destinationSideIndex - startSideIndex);
	int direction = startSideIndex < destinationSideIndex ? CLOCKWISE : COUNTERCLOCKWISE;
	direction = (dist < NUM_SIDES/2) ? direction : -direction;

	return direction;
}

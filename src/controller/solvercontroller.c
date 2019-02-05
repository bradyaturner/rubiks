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

int shortestDistanceToFace(Rubiks *rubiks, int faceToRotate,
	int startSideFace, int desiredSideFace
);

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

int whiteCrossFaces[4] = {BACK_FACE, LEFT_FACE, RIGHT_FACE, FRONT_FACE};
void solveWhiteCross(Rubiks *rubiks) {
	log_info("%s\n", "Inside solveWhiteCross()");

	for (int i=0; i<4; i++) {
		int id = whiteCrossCubeIds[i];
		Cube *cube = rc_getCubeById(rubiks, id);
		int correctPos = cube_checkPosition(cube);
		int correctRot = cube_checkRotation(cube);
		if (correctPos && correctRot) {
			log_info("Cube %i is solved.\n", id);
			continue;
		}

		int currentSideFace = -1;
		for (int faceIndex=0; faceIndex<4; faceIndex++) {
			if (rc_checkCubeInFace(cube, whiteCrossFaces[faceIndex])) {
				currentSideFace = whiteCrossFaces[faceIndex];
				break;
			}
		}
		if (currentSideFace == -1) {
			log_fatal("Did not find side face for cube: %i\n", id);
			exit(1);
		}

		int currentOtherFace = -1;
		for (int faceIndex=0; faceIndex<NUM_FACES; faceIndex++) {
			if (faceIndex == currentSideFace) {
				continue;
			}
			if (rc_checkCubeInFace(cube, faceIndex)) {
				currentOtherFace = faceIndex;
				break;
			}
		}
		if (currentOtherFace == -1) {
			log_fatal("Did not find second face for cube: %i, current side face: %i\n", id, currentSideFace);
			exit(1);
		}

		if (correctPos && !correctRot) {
			log_info("Cube %i is in correct position, but incorrect rotation\n", id);
			Step s = {currentSideFace, CLOCKWISE};
			enqueue(&queue, s);
			s = (Step){faceData[currentSideFace].neighbors[RIGHT], COUNTERCLOCKWISE};
			enqueue(&queue, s);

			s = (Step){DOWN_FACE, COUNTERCLOCKWISE};
			enqueue(&queue, s);
			// Cube will be in DOWN_FACE

			s = (Step){faceData[currentSideFace].neighbors[RIGHT], CLOCKWISE};
			enqueue(&queue, s);
			// Cube will be in (correct face), DOWN_FACE

			s = (Step){currentSideFace, COUNTERCLOCKWISE};
			enqueue(&queue, s);
			enqueue(&queue, s);
		} else if (rc_checkCubeInFace(cube, whiteCrossFaces[i])) {
			log_info("Cube %i is in correct face, but not correct position\n", id);
			// Once in correct face, rotate face 2x to get into UP_FACE
			int startFace = (currentSideFace == whiteCrossFaces[i]) ? currentOtherFace : currentSideFace;
			int distance = shortestDistanceToFace(rubiks, whiteCrossFaces[i], startFace, UP_FACE);
			int direction = distance/abs(distance);
			distance = abs(distance);
			log_info("Rotating %c %i times in %i direction to get from %c to %c\n",
				faceData[whiteCrossFaces[i]].name, distance, direction,
				faceData[startFace].name, faceData[UP_FACE].name
			);
			for (int count=0; count<distance; count++) {
				Step s = {whiteCrossFaces[i], direction};
				enqueue(&queue, s);
			}
		} else if (!correctPos) {
			// TODO if in correct face already, rotate to top
			log_info("Cube %i is in incorrect position\n", id);
			// Rotate to bottom face first
			int faceRotated = -1;
			int timesRotated = 0;
			int directionRotated = 0;
			if (!rc_checkCubeInFace(cube, DOWN_FACE)) {
				int distance = shortestDistanceToFace(rubiks, currentSideFace, currentOtherFace, DOWN_FACE);
				int direction = distance/abs(distance);
				distance = abs(distance);
				log_info("Rotating %c %i times in %i direction to get from %c to %c\n",
					faceData[currentSideFace].name, distance, direction,
					faceData[currentOtherFace].name, faceData[DOWN_FACE].name
				);
				for (int count=0; count<distance; count++) {
					Step s = {currentSideFace, direction};
					enqueue(&queue, s);
				}
				currentOtherFace = DOWN_FACE;
				faceRotated = currentSideFace;
				timesRotated = direction;
				directionRotated = direction;
				// TODO revert these rotations after rotating bottom face
			}
			// Once in DOWN_FACE, rotate DOWN_FACE to get on correct side face
			if (!rc_checkCubeInFace(cube, whiteCrossFaces[i])) {
				int distance = shortestDistanceToFace(rubiks, currentOtherFace, currentSideFace, whiteCrossFaces[i]);
				int direction = distance/abs(distance);
				distance = abs(distance);
				log_info("Rotating DOWN_FACE %i times in %i direction to get from %c to %c\n",
					distance, direction, faceData[currentSideFace].name, faceData[whiteCrossFaces[i]].name
				);
				for (int count=0; count<distance; count++) {
					Step s = {currentOtherFace, direction};
					enqueue(&queue, s);
				}
				currentSideFace = whiteCrossFaces[i];
			}

			if (timesRotated) {
				for (int count=0; count<timesRotated; count++) {
					Step s = {faceRotated, directionRotated*-1};
					enqueue(&queue, s);
				}
			}
			// Rotation to UP_FACE handled by (in correct face) case
		}
		break; // if reached here then didn't hit 'continue' above
	}

}

void solveWhiteCorners(Rubiks *rubiks) {
	log_info("%s\n", "Inside solveWhiteCorners()");
//	log_info("%s\n", "EXITING");
//	exit(1);

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
int shortestDistanceToFace(Rubiks *rubiks, int faceToRotate,
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
	dist = (dist < NUM_SIDES/2) ? dist : NUM_SIDES-dist;

	return direction*dist;
}

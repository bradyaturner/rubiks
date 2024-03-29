#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include "rubiks.h"
#include "view/rubiksview.h"
#include "controller/rubikscontroller.h"
#include "controller/solvercontroller.h"
#include "cube.h"
#include "view/cubeview.h"
#include "vector.h"
#include "logger.h"

#define ROTATION_SPEED_DEFAULT 18
#define ROTATION_SPEED_MAX 25
#define ROTATION_SPEED_MIN 0.1
#define ROTATION_SPEED_INCREMENT 1

int animationsOn = 1;

// OpenGL/GLFW functions
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void display();

// Control functions
void resetCameraRotation();
void printHelpText();
void increaseRotationSpeed();
void decreaseRotationSpeed();
void resetRotationSpeed();
void rc_toggleAnimations();
void toggleDemoMode();
void toggleAutorotate();

// Drawing functions
void drawAxisLines();

// Debug functions
void resetDebugInfo();

Vec2d rotate = {-30, 30};

GLFWwindow *window;

Rubiks rubiksCube;
int solverEnabled = 0;
int debug = 0;
int demoMode = 0;
int autorotate = 0;
double rotationSpeed = ROTATION_SPEED_DEFAULT;

void rc_toggleAnimations() {
	animationsOn = !animationsOn;
	log_info("Animations %s", animationsOn ? "ON" : "OFF");
}

void toggleDemoMode() {
	demoMode = !demoMode;
	log_info("Demo mode %s", demoMode ? "ON" : "OFF");
}

void toggleAutorotate() {
	autorotate = !autorotate;
	log_info("Autorotate %s", autorotate ? "ON" : "OFF");
}

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Rotate when user changes rotate.x and rotate.y
	glRotatef( rotate.x, 1.0, 0.0, 0.0 );
	glRotatef( rotate.y, 0.0, 1.0, 0.0 );

	rc_updateFaceRotations(&rubiksCube, rotationSpeed);
	rc_draw(&rubiksCube);
	if (debug) {
		drawAxisLines();
	}

	glFlush();
}

void resetRotationSpeed() {
	rotationSpeed = ROTATION_SPEED_DEFAULT;
}

void increaseRotationSpeed() {
	rotationSpeed = mind(rotationSpeed + ROTATION_SPEED_INCREMENT, ROTATION_SPEED_MAX);
	printf("Increasing rotation speed: %f\n", rotationSpeed);
}

void decreaseRotationSpeed() {
	rotationSpeed = maxd(rotationSpeed - ROTATION_SPEED_INCREMENT, ROTATION_SPEED_MIN);
	printf("Decreasing rotation speed: %f\n", rotationSpeed);
}

void drawAxisLines() {
	glBegin(GL_LINES);
	// x axis is RED
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);
	// y axis is GREEN
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);
	// z axis is BLUE
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();
}

void resetDebugInfo() {
	for (int i=0; i<NUM_CUBES; i++) {
		Cube* cube = &rubiksCube.cubes[i];
		log_info("Cube #%i at position: %i, quaternion: {%f, %f, %f, %f}",
			i, cube->position, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	}
	for (int i=0; i<NUM_FACES; i++) {
		char faceStr[FACE_SIZE+1];
		faceStr[FACE_SIZE] = '\0';
		rc_getFaceColors(&rubiksCube, i, faceStr);
		printf("FACE: %c: %s\n", faceData[i].name, faceStr);
	}
	char rubiksStr[FACE_SIZE*NUM_FACES+1];
	rubiksStr[FACE_SIZE*NUM_FACES] = '\0';
	rc_serialize(&rubiksCube, rubiksStr);
	printf("RUBIKS: %s\n", rubiksStr);
}

void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS && action != GLFW_REPEAT) {
		return;
	}

	log_debug("Key pressed: %i", key);
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, 1);
			break;
		case GLFW_KEY_SPACE:
			solverEnabled = !solverEnabled;
			break;
		case GLFW_KEY_S:
			rc_shuffle(&rubiksCube, 20);
			break;
		case GLFW_KEY_I:
			rc_reset(&rubiksCube);
			break;
		case GLFW_KEY_H:
			printHelpText();
			break;
		case GLFW_KEY_P:
			resetDebugInfo();
			break;
		case GLFW_KEY_C:
			if (mods==0) {
				debug = !debug;
			} else if (mods == GLFW_MOD_SHIFT) {
				toggleDemoMode();
			}
			break;
		case GLFW_KEY_A:
			if (mods==0) {
				rc_toggleAnimations();
			} else if (mods == GLFW_MOD_SHIFT) {
				toggleAutorotate();
			}
			break;
		case GLFW_KEY_EQUAL:
			if (mods==0) {
				resetRotationSpeed();
			} else if (mods == GLFW_MOD_SHIFT) {
				increaseRotationSpeed();
			}
			break;
		case GLFW_KEY_MINUS:
			decreaseRotationSpeed();
			break;
		case 48:
			resetCameraRotation();
			break;
		case GLFW_KEY_RIGHT:
			rotate.y += 5;
			break;
		case GLFW_KEY_LEFT:
			rotate.y -= 5;
			break;
		case GLFW_KEY_UP:
			rotate.x += 5;
			break;
		case GLFW_KEY_DOWN:
			rotate.x -= 5;
			break;
		case GLFW_KEY_1:
		case GLFW_KEY_L:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, LEFT_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, LEFT_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
		case GLFW_KEY_2:
		case GLFW_KEY_R:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, RIGHT_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, RIGHT_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
		case GLFW_KEY_3:
		case GLFW_KEY_D:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, DOWN_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, DOWN_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
		case GLFW_KEY_4:
		case GLFW_KEY_U:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, UP_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, UP_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
		case GLFW_KEY_5:
		case GLFW_KEY_F:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, FRONT_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, FRONT_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
		case GLFW_KEY_6:
		case GLFW_KEY_B:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, BACK_FACE, CLOCKWISE, !animationsOn);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, BACK_FACE, COUNTERCLOCKWISE, !animationsOn);
				}
			}
			break;
	}
}

void resetCameraRotation() {
	rotate.x = 0;
	rotate.y = 0;
}

void printHelpText() {
	printf("Controls:\n");

	printf("\tGeneral:\n");
	printf("\t\th: print this dialog\n");
	printf("\t\tq/ESC: quit\n");
	printf("\t\tc: enable/disable debug mode\n");
	printf("\t\tC: enable/disable demo mode\n");
	printf("\t\ta: enable/disable animations\n");
	printf("\t\tA: enable/disable autorotate\n");
	printf("\t\t+: increase rotation speed\n");
	printf("\t\t-: decrease rotation speed\n");
	printf("\t\t=: reset rotation speed\n");
	printf("\t\tp: print debug info\n");

	printf("\tCamera controls:\n");

	printf("\t\t0: reset rotation\n");
	printf("\t\tleft/right: rotate camera y-axis\n");
	printf("\t\tup/down: rotate camera x-axis\n");

	printf("\tRotation keys (key rotates clockwise, shift+key rotates ccw):\n");
	printf("\t\ts: shuffle\n");
	printf("\t\ti: initialize\n");
	printf("\t\tl/1: left face\n");
	printf("\t\tr/2: right face\n");
	printf("\t\td/3: down face\n");
	printf("\t\tu/4: up face\n");
	printf("\t\tf/5: front face\n");
	printf("\t\tb/6: back face\n");
}

void glapp_init() {
	// TODO move application initialization here
	rc_initialize(&rubiksCube);
}

void glapp_loadState(char *fileName) {
	rc_initialize(&rubiksCube);
	printf("Loading state from file: %s\n", fileName);
	FILE *fp;
	fp = fopen(fileName, "r");
	int c;
	char state[2000];
	int index = 0;
	while ((c = getc(fp)) != EOF) {
		state[index] = c;
		index++;
	}
	state[index] = '\0';
	rc_deserializeState(&rubiksCube, state);
}

int glapp_run(){
	if (!glfwInit()) {
		return EXIT_FAILURE;
	}

	window = glfwCreateWindow(800, 800, "Rubik's Cube", NULL, NULL);
	if (!window) {
		log_fatal("%s","Problem creating window!");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, keyboardHandler);
	glClearColor(0.85, 0.85, 0.85, 0.0);

	printHelpText();

	solver_init();

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		if (solverEnabled) {
			int solved = solver_checkSolved(&rubiksCube);
			if (solved && demoMode) {
				rc_shuffle(&rubiksCube, 20);
			} else if (solved && !demoMode) {
				solverEnabled = 0;
			} else if (!solved) {
				solver_solve(&rubiksCube, animationsOn);
			}
		}
		if (autorotate) {
			rotate.y += 1;
		}
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}

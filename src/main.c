#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "rubiks.h"
#include "view/rubiksview.h"
#include "cube.h"
#include "view/cubeview.h"
#include "vector.h"
#include "logger.h"

#define ROTATION_SPEED_DEFAULT 18
#define ROTATION_SPEED_MAX 25
#define ROTATION_SPEED_MIN 0.1
#define ROTATION_SPEED_INCREMENT 1

// OpenGL/GLFW functions
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void display();

// Control functions
void resetCameraRotation();
void printHelpText();
void increaseRotationSpeed();
void decreaseRotationSpeed();
void resetRotationSpeed();

// Drawing functions
void drawAxisLines();

// Debug functions
void resetDebugInfo();

Vec2d rotate = {0, 0};

GLFWwindow *window;

Rubiks rubiksCube;
int debug = 0;
int demoMode = 0;
double rotationSpeed = ROTATION_SPEED_DEFAULT;

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
		log_info("Cube #%i at position: %i, quaternion: {%f, %f, %f, %f}\n",
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

	log_debug("Key pressed: %i\n", key);
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, 1);
			break;
		case GLFW_KEY_S:
			rc_shuffle(&rubiksCube, 20);
			break;
		case GLFW_KEY_R:
			rc_reset(&rubiksCube);
			break;
		case GLFW_KEY_H:
			printHelpText();
			break;
		case GLFW_KEY_P:
			resetDebugInfo();
			break;
		case GLFW_KEY_D:
			if (mods==0) {
				debug = !debug;
			} else if (mods == GLFW_MOD_SHIFT) {
				demoMode = !demoMode;
			}
			break;
		case GLFW_KEY_A:
			animationsOn = !animationsOn;
			printf("Animations %s\n", animationsOn ? "ON" : "OFF");
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
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, LEFT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, LEFT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_2:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, RIGHT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, RIGHT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_3:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, BOTTOM_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, BOTTOM_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_4:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, TOP_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, TOP_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_5:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, FRONT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, FRONT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_6:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rc_beginFaceRotation(&rubiksCube, BACK_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rc_beginFaceRotation(&rubiksCube, BACK_FACE, COUNTERCLOCKWISE);
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
	printf("\t\td: enable/disable debug mode\n");
	printf("\t\tD: enable/disable demo mode\n");
	printf("\t\ta: enable/disable animations\n");
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
	printf("\t\tr: reset\n");
	printf("\t\t1: left face\n");
	printf("\t\t2: right face\n");
	printf("\t\t3: bottom face\n");
	printf("\t\t4: top face\n");
	printf("\t\t5: front face\n");
	printf("\t\t6: back face\n");
}

int main( int argc, char* argv[] ){
	rc_initialize(&rubiksCube);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(800, 800, "Rubik's Cube", NULL, NULL);
	if (!window) {
		log_fatal("%s\n","Problem creating window!");
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, keyboardHandler);
	glClearColor(0.85, 0.85, 0.85, 0.0);

	printHelpText();

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		display();

		if (demoMode) {
			if(!rc_isRotating()) {
				rc_beginFaceRotation(&rubiksCube, rand()%NUM_FACES, rand()%2==0 ? 1:-1);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}



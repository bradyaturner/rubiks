#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "rubiks.h"
#include "cube.h"
#include "vector.h"
#include "quaternion.h"
#include "logger.h"

// OpenGL/GLFW functions
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void display();

// Control functions
void resetCameraRotation();
void printHelpText();

// Drawing functions
void drawAxisLines();
void drawRubiksCube();
Vec3f determineCubeCoord(Cube *cube);
void drawCube(Cube cube, Vec3f coords);
void drawNormalCube(const Cube cube, int useColor);
void drawNormalSquare(int x, int y, int z);

// Debug functions
void resetDebugInfo();

double rotate_y=0;
double rotate_x=0;

double cubeWidth = 0.3;

GLFWwindow *window;

Rubiks rubiksCube;
int printed = 1;
int debug = 0;

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Rotate when user changes rotate_x and rotate_y
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );

	drawRubiksCube();
	if (debug) {
		drawAxisLines();
	}

	glFlush();
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

Vec3f determineCubeCoord(Cube *cube) {
	// TODO there has to be a smarter way to do this math
	int xPos, yPos, zPos;

	if (cubeInFace(cube, LEFT_FACE)) {
		xPos = -1;
	} else if (cubeInFace(cube, RIGHT_FACE)) {
		xPos = 1;
	} else {
		xPos = 0;
	}

	if (cubeInFace(cube, BOTTOM_FACE)) {
		yPos = -1;
	} else if (cubeInFace(cube, TOP_FACE)) {
		yPos = 1;
	} else {
		yPos = 0;
	}

	if (cubeInFace(cube, FRONT_FACE)) {
		zPos = -1;
	} else if (cubeInFace(cube, BACK_FACE)) {
		zPos = 1;
	} else {
		zPos = 0;
	}
	if (!printed) {
		log_info("Drawing cube %i at postition: %i, offset=[ %i, %i, %i ]\n", cube->id, cube->position, xPos, yPos, zPos);
	}

	Vec3f cubeCoord = {xPos*cubeWidth, yPos*cubeWidth, zPos*cubeWidth};
	return cubeCoord;
}

void drawRubiksCube(){
	for(int i=0; i<NUM_CUBES; i++){
		Cube *cube = findCube(&rubiksCube, i);
		Vec3f cubeCoord = determineCubeCoord(cube);
		drawCube(*cube, cubeCoord);
	}
	printed = 1;
}

void resetDebugInfo() {
	printed = 0;
	for (int i=0; i<NUM_CUBES; i++) {
		Cube* cube = &rubiksCube.cubes[i];
		log_info("Cube #%i at position: %i, quaternion: {%f, %f, %f, %f}\n",
			i, cube->position, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	}
	for (int i=0; i<NUM_FACES; i++) {
		char faceStr[FACE_SIZE+1];
		faceStr[FACE_SIZE] = '\0';
		getFaceColors(&rubiksCube, i, faceStr);
		printf("FACE: %c: %s\n", faceData[i].name, faceStr);
	}
	char rubiksStr[FACE_SIZE*NUM_FACES+1];
	rubiksStr[FACE_SIZE*NUM_FACES] = '\0';
	serializeRubiks(&rubiksCube, rubiksStr);
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
			shuffle(&rubiksCube, 20);
			break;
		case GLFW_KEY_R:
			reset(&rubiksCube);
			break;
		case GLFW_KEY_H:
			printHelpText();
			break;
		case GLFW_KEY_P:
			resetDebugInfo();
			break;
		case GLFW_KEY_D:
			debug = !debug;
			break;
		case 48:
			resetCameraRotation();
			break;
		case GLFW_KEY_RIGHT:
			rotate_y += 5;
			break;
		case GLFW_KEY_LEFT:
			rotate_y -= 5;
			break;
		case GLFW_KEY_UP:
			rotate_x += 5;
			break;
		case GLFW_KEY_DOWN:
			rotate_x -= 5;
			break;
		case GLFW_KEY_1:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, LEFT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, LEFT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_2:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, RIGHT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, RIGHT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_3:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, BOTTOM_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, BOTTOM_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_4:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, TOP_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, TOP_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_5:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, BACK_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, BACK_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_6:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, FRONT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, FRONT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
	}
}

void resetCameraRotation() {
	rotate_x = 0;
	rotate_y = 0;
}

void printHelpText() {
	printf("Controls:\n");

	printf("\tGeneral:\n");
	printf("\t\th: print this dialog\n");
	printf("\t\tq/ESC: quit\n");
	printf("\t\td: enable/disable debug mode\n");
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
	initializeRubiks(&rubiksCube);

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

	int printedSuccess = 0;
	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		display();

		if (!isRubiksSolved(&rubiksCube)) {
			printedSuccess = 0;
		} else {
			if (!printedSuccess) {
				printf("SOLVED!!!\n");
				printedSuccess = 1;
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void drawCube(Cube cube, Vec3f coords) {
	glPushMatrix();

	glTranslatef(coords.x, coords.y, coords.z);
	glScalef(cubeWidth, cubeWidth, cubeWidth);
	glMultMatrixf(quatToMatrix(&cube.quat));

	// Draw outline
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1,-1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((GLfloat)8); // this should be proportional to cube size
	glColor3f(0.2, 0.2, 0.2); // line color
	drawNormalCube(cube, 0);
	glDisable(GL_POLYGON_OFFSET_LINE);

	// Draw solid polygons
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1); // just guessing on these values
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawNormalCube(cube, 1);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

void drawNormalCube(const Cube cube, int useColor) {
	GLfloat vertices[] =
	{
		-0.5, -0.5, -0.5,		-0.5, -0.5, 0.5,		-0.5, 0.5, 0.5,		-0.5, 0.5, -0.5, // left
		0.5, -0.5, -0.5,		0.5, -0.5,  0.5,		0.5,  0.5, 0.5,		0.5,  0.5, -0.5, // right
		-0.5, -0.5, -0.5,		-0.5, -0.5,  0.5,		0.5, -0.5, 0.5,		0.5, -0.5, -0.5, // bottom
		-0.5,  0.5, -0.5,		-0.5,  0.5,  0.5,		0.5, 0.5, 0.5,		0.5,  0.5, -0.5, // top
		-0.5, -0.5, -0.5,		-0.5,  0.5, -0.5,		0.5, 0.5, -0.5,		0.5, -0.5, -0.5, // front
		-0.5, -0.5,  0.5,		-0.5,  0.5,  0.5,		0.5, 0.5, 0.5,		0.5, -0.5,  0.5  // back
	};

	float *colors = getColorArray(cube);

	glEnableClientState(GL_VERTEX_ARRAY);
	if (useColor)
		glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if (useColor)
		glColorPointer(3, GL_FLOAT, 0, colors);

	glDrawArrays(GL_QUADS, 0, 24);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (useColor)
		glDisableClientState(GL_COLOR_ARRAY);
}

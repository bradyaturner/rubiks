#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "singlecube.h"
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
void drawCube(Cube cube, Vec3f coords);
void drawNormalCube(const Cube cube, int useColor);
void drawNormalSquare(int x, int y, int z);

// Debug functions
void resetDebugInfo();

double rotate_y=0; 
double rotate_x=0;

double cubeWidth = 0.7;

GLFWwindow *window;

Cube myCube;
int cubeDebugInfo = 0;

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Rotate when user changes rotate_x and rotate_y
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );

	Vec3f pos = {0.0,0.0,0.0};
	drawCube(myCube, pos);

	drawAxisLines();
	cubeDebugInfo = 0;
	glFlush();
}

void drawAxisLines() {
	glBegin(GL_LINES);
	// -x axis is RED
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(-10.0, 0.0, 0.0);

	// +x axis is ORANGE
	glColor3f(1.0, 0.65, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	// -y axis is YELLOW
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, -10.0, 0.0);

	// -y axis is BLACK
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);

	// -z axis is BLUE
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, -10.0);

	// z axis is GREEN
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();
}

void resetDebugInfo() {
	cubeDebugInfo = 0;
	log_debug("Cube at position: %i, quat: (%f, %f, %f, %f)\n",
		myCube.position, myCube.quat.x, myCube.quat.y, myCube.quat.z, myCube.quat.w
	);
}

void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS && action != GLFW_REPEAT) {
		return;
	}

	Vec3i ydeg = {5,0,0};
	Vec3i xdeg = {0,5,0};
	Vec3i zdeg = {0,0,5};
	log_debug("Key pressed: %i\n", key);
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, 1);
			break;
		case GLFW_KEY_P:
			resetDebugInfo();
			break;
		case GLFW_KEY_H:
			printHelpText();
			break;
		case GLFW_KEY_T:
			rotateCube(&myCube, ydeg);
			break;
		case GLFW_KEY_Y:
			rotateCube(&myCube, xdeg);
			break;
		case GLFW_KEY_U:
			rotateCube(&myCube, zdeg);
			break;
		case GLFW_KEY_R:
			resetRotation(&myCube);
			break;
		case 48:
			resetCameraRotation();
			break;
		case GLFW_KEY_1:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, LEFT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, LEFT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_2:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, RIGHT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, RIGHT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_3:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, BOTTOM_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, BOTTOM_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_4:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, TOP_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, TOP_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_5:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, FRONT_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, FRONT_FACE, COUNTERCLOCKWISE);
				}
			}
			break;
		case GLFW_KEY_6:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateSingleCube(&myCube, BACK_FACE, CLOCKWISE);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateSingleCube(&myCube, BACK_FACE, COUNTERCLOCKWISE);
				}
			}
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
	printf("\t\tr: reset\n");
	printf("\t\t1: left face\n");
	printf("\t\t2: right face\n");
	printf("\t\t3: bottom face\n");
	printf("\t\t4: top face\n");
	printf("\t\t5: back face\n");
	printf("\t\t6: front face\n");
}

int main( int argc, char* argv[] ){
	initializeCube(&myCube, 0, 0);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(640, 480, "Rubik's Cube", NULL, NULL);
	if (!window) {
		log_fatal("%s\n", "Problem creating window!");
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, keyboardHandler);
	glClearColor(0.85, 0.85, 0.85, 0.0);

	logState(&myCube);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
		display();
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

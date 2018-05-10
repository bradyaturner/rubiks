#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "rubiks.h"
#include "cube.h"
#include "vector.h"
#include "quaternion.h"

// OpenGL/GLFW functions
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void display();

// Control functions
void resetCameraRotation();

// Drawing functions
void drawAxisLines();
void drawRubiksCube();
void drawCube(Cube cube, Vec3f coords);
void drawNormalCube(Cube cube, int useColor);
void drawNormalSquare(int x, int y, int z);

// Debug functions
void resetDebugInfo();

double rotate_y=0; 
double rotate_x=0;

double cubeWidth = 0.3;

GLFWwindow *window;

Rubiks rubiksCube;
int printed = 0;
int debug = 0;
const Vec3i origin = {0, 0, 0};

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

void drawRubiksCube(){
	// TODO there has to be a smarter way to do this math
	for(int i=0; i<27; i++){
		int index = findCube(&rubiksCube, i);
		Cube *cube = &rubiksCube.cubes[index];
		int position = cube->position + 1;
		int yPos = (((position-1) / 9) - 1) * -1;
		int xPos = (position % 3) == 0 ? 1 : (position%3)-2;
		int zPos;
		if ((position>=1 && position<=3) || (position>=10 && position<=12) || (position>=19 && position<=21)) {
			zPos = -1;
		} else if ((position>=4 && position<=6) || (position>=13 && position<=15) || (position>=22 && position<=24)) {
			zPos = 0;
		} else {
			zPos = 1;
		}
		if (!printed) {
			printf("Drawing cube %i at postition: %i, offset=[ %i, %i, %i ]\n", index, position-1, xPos, yPos, zPos);
		}
		Vec3f cubeCoord = {xPos*cubeWidth, yPos*cubeWidth, zPos*cubeWidth};

		int cubeNum = findCube(&rubiksCube, i);
		drawCube(rubiksCube.cubes[cubeNum], cubeCoord);
	}
	printed = 1;
}

void resetDebugInfo() {
	printed = 0;
	for (int i=0; i<27; i++) {
		Cube* cube = &rubiksCube.cubes[i];
		printf("Cube #%i at position: %i, quaternion: {%f, %f, %f, %f}\n",
			i, cube->position, cube->quat.x, cube->quat.y, cube->quat.z, cube->quat.w);
	}
}

void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS && action != GLFW_REPEAT) {
		return;
	}

	printf("Key pressed: %i\n", key);
	switch (key)
	{
		case 27:
		case 81:
		case 113:
			glfwSetWindowShouldClose(window, 1);
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
					rotateCubeFace(&rubiksCube, 1, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 1, -1);
				}
			}
			break;
		case GLFW_KEY_2:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, 2, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 2, -1);
				}
			}
			break;
		case GLFW_KEY_3:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, 3, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 3, -1);
				}
			}
			break;
		case GLFW_KEY_4:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, 4, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 4, -1);
				}
			}
			break;
		case GLFW_KEY_5:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, 5, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 5, -1);
				}
			}
			break;
		case GLFW_KEY_6:
			if (action == GLFW_PRESS) {
				if (mods==0) {
					rotateCubeFace(&rubiksCube, 6, 1);
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateCubeFace(&rubiksCube, 6, -1);
				}
			}
			break;
	}
}

void resetCameraRotation() {
	rotate_x = 0;
	rotate_y = 0;
}

int main( int argc, char* argv[] ){
	initializeRubiks(&rubiksCube);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(640, 480, "Rubik's Cube", NULL, NULL);
	if (!window) {
		printf("Problem creating window!\n");
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, keyboardHandler);
	glClearColor(0.85, 0.85, 0.85, 0.0);

	while (!glfwWindowShouldClose(window)) {
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

void drawSquare(Vec3f lr, Vec3f ur, Vec3f ul, Vec3f ll, RGB3f color) {
	glBegin(GL_QUADS);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(lr.x, lr.y, lr.z);
	glVertex3f(ur.x, ur.y, ur.z);
	glVertex3f(ul.x, ul.y, ul.z);
	glVertex3f(ll.x, ll.y, ll.z);
	glEnd();

	// Black outline for square
	// TODO this is broken on one corner (yellow-red/front-bottom edge)
	glLineWidth((GLfloat)5);
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(lr.x, lr.y, lr.z);
	glVertex3f(ur.x, ur.y, ur.z);
	glVertex3f(ul.x, ul.y, ul.z);
	glVertex3f(ll.x, ll.y, ll.z);
	glEnd();
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

void drawNormalSquare(int x, int y, int z) {
	if (x != 0) {
		glVertex3f(x*0.5, 0.5, 0.5);
		glVertex3f(x*0.5, -0.5, 0.5);
		glVertex3f(x*0.5, -0.5, -0.5);
		glVertex3f(x*0.5, 0.5, -0.5);
	} else if (y != 0) {
		glVertex3f(0.5, y*0.5, 0.5);
		glVertex3f(-0.5, y*0.5, 0.5);
		glVertex3f(-0.5, y*0.5, -0.5);
		glVertex3f(0.5, y*0.5, -0.5);
	} else  {
		glVertex3f(0.5, 0.5, z*0.5);
		glVertex3f(0.5, -0.5, z*0.5);
		glVertex3f(-0.5, -0.5, z*0.5);
		glVertex3f(-0.5, 0.5, z*0.5);
	}
}

// draws a 1x1x1 cube centered about the origin with no rotation
void drawNormalCube(Cube cube, int useColor) {
	glBegin(GL_QUADS);

	// top
	if (useColor)
		glColor3f(cube.top.color.red, cube.top.color.green, cube.top.color.blue);
	drawNormalSquare(0,1,0);

	// bottom
	if (useColor)
		glColor3f(cube.bottom.color.red, cube.bottom.color.green, cube.bottom.color.blue);
	drawNormalSquare(0, -1, 0);

	// left
	if (useColor)
		glColor3f(cube.left.color.red, cube.left.color.green, cube.left.color.blue);
	drawNormalSquare(-1,0,0);

	// right
	if (useColor)
	glColor3f(cube.right.color.red, cube.right.color.green, cube.right.color.blue);
	drawNormalSquare(1,0,0);

	// front
	if (useColor)
	glColor3f(cube.front.color.red, cube.front.color.green, cube.front.color.blue);
	drawNormalSquare(0,0,-1);

	// back
	if (useColor)
		glColor3f(cube.back.color.red, cube.back.color.green, cube.back.color.blue);
	drawNormalSquare(0,0,1);

	glEnd();
}

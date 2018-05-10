#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "singlecube.h"
#include "cube.h"
#include "vector.h"
#include "quaternion.h"

void display();
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void resetDebugInfo();
void resetCameraRotation();
void drawAxisLines();

void drawNormalSquare(int x, int y, int z);
void drawNormalCube(Cube *cube, int useColor);
void drawCube(Cube *cube, Vec3f coords);

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
	drawCube(&myCube, pos);

	drawAxisLines();
	cubeDebugInfo = 0;
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

void resetDebugInfo() {
	cubeDebugInfo = 0;
	printf("Cube at position: %i, quat: (%f, %f, %f, %f)\n",
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
		case GLFW_KEY_R:
			rotateCube(&myCube, ydeg);
			break;
		case GLFW_KEY_T:
			rotateCube(&myCube, xdeg);
			break;
		case GLFW_KEY_Y:
			rotateCube(&myCube, zdeg);
			break;
		case GLFW_KEY_DELETE:
			resetRotation(&myCube);
			break;
		case 48:
			resetCameraRotation();
			break;
		case GLFW_KEY_1:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 1, 1); // top layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 1, -1); // top layer ccw
				}
			}
			break;
		case GLFW_KEY_2:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 2, 1); // bottom layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 2, -1); // bottom layer ccw
				}
			}
			break;
		case GLFW_KEY_3:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 5, 1); // front layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 5, -1); // front layer ccw
				}
			}
			break;
		case GLFW_KEY_4:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 6, 1); // 4 back layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 6, -1); // 4 back layer ccw
				}
			}
			break;
		case GLFW_KEY_5:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 4, 1); // 5 right layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 4, -1); // 5 right layer ccw
				}
			}
			break;
		case GLFW_KEY_6:
			if ( action == GLFW_PRESS) {
				if (mods == 0) {
					rotateF(&myCube, 3, 1); // 6 left layer cw
				} else if (mods == GLFW_MOD_SHIFT) {
					rotateF(&myCube, 3, -1); // 6 left layer ccw
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

int main( int argc, char* argv[] ){
	initializeCube(&myCube);

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

void drawCube(Cube *cube, Vec3f coords) {
	glPushMatrix();
	if (cubeDebugInfo) {
		printf("Drawing cube 0 with quaternion: {%f, %f, %f, %f}\n",
			cube->quat.x,
			cube->quat.y,
			cube->quat.z,
			cube->quat.w
		);
	}
	glTranslatef(coords.x, coords.y, coords.z);
	// glScalef( 2.0, 2.0, 0.0 ); use this instead of adding/subtracting halfWidth

	// Rotate to draw cube
	glMultMatrixf(quatToMatrix(&cube->quat));

	// Draw outline
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1,-1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((GLfloat)8);
	glColor3f(0.2, 0.2, 0.2); // line color
	drawNormalCube(cube, 0);
	glDisable(GL_POLYGON_OFFSET_LINE);

	// Draw solid polygons
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1);
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
void drawNormalCube(Cube *cube, int useColor) {
	glBegin(GL_QUADS);

	// top
	if (useColor)
		glColor3f(cube->top.color.red, cube->top.color.green, cube->top.color.blue);
	drawNormalSquare(0,1,0);

	// bottom
	if (useColor)
		glColor3f(cube->bottom.color.red, cube->bottom.color.green, cube->bottom.color.blue);
	drawNormalSquare(0, -1, 0);

	// left
	if (useColor)
		glColor3f(cube->left.color.red, cube->left.color.green, cube->left.color.blue);
	drawNormalSquare(-1,0,0);

	// right
	if (useColor)
		glColor3f(cube->right.color.red, cube->right.color.green, cube->right.color.blue);
	drawNormalSquare(1,0,0);

	// front
	if (useColor)
		glColor3f(cube->front.color.red, cube->front.color.green, cube->front.color.blue);
	drawNormalSquare(0,0,-1);

	// back
	if (useColor)
		glColor3f(cube->back.color.red, cube->back.color.green, cube->back.color.blue);
	drawNormalSquare(0,0,1);
	glEnd();
}


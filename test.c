#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "singlecube.h"
#include "cube.h"
#include "vector.h"

typedef struct{
    Vec3f ll;
    Vec3f ur;
} Rectangle;

void initialize();
void display();
void drawCube();
void drawRectangle(Vec3f ll, Vec3f ur, RGB3f color);
void specialKeys( int key, int x, int y );
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void resetDebugInfo();
void resetCameraRotation();
void drawAxisLines();

double rotate_y=0; 
double rotate_x=0;

double cubeWidth = 0.7;

GLFWwindow *window;

Cube myCube;
int printed = 0;
int cubeDebugInfo = 0;
const Vec3i origin = {0, 0, 0};

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Rotate when user changes rotate_x and rotate_y
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );

	drawCube();
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
	printed = 0;
	cubeDebugInfo = 0;
	printf("Cube at position: %i, rotation: (%i, %i, %i)\n",
		myCube.position, myCube.rotation.x, myCube.rotation.y, myCube.rotation.z
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

void initialize() {
	initializeCube(&myCube);
}

int main( int argc, char* argv[] ){
	initialize();
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
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

void drawSquare(Vec3f lr, Vec3f ur, Vec3f ul, Vec3f ll, RGB3f color) {
	glBegin(GL_QUADS);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(lr.x, lr.y, lr.z);
	glVertex3f(ur.x, ur.y, ur.z);
	glVertex3f(ul.x, ul.y, ul.z);
	glVertex3f(ll.x, ll.y, ll.z);
	glEnd();
}

void drawRectangle(Vec3f ll, Vec3f ur, RGB3f color){
	// TODO rewrite with the assumption we will use translate/rotate/scale functions to position and resize
	Vec3f ul;
	Vec3f lr;

	if (ll.z == ur.z) { // rectangle in z-plane
		ul.x = ll.x;
		ul.y = ur.y;
		ul.z = ll.z; // all z equal
		lr.x = ur.x;
		lr.y = ll.y;
		lr.z = ll.z; // all z equal
	} else if (ll.x == ur.x ) { // rectangle in x-plane
		ul.z = ll.z;
		ul.y = ur.y;
		ul.x = ll.x; // all x equal
		lr.z = ur.z;
		lr.y = ll.y;
		lr.x = ll.x; // all x equal
	} else { // rectangle in y-plane
		ul.x = ll.x;
		ul.z = ur.z;
		ul.y = ll.y; // all y equal
		lr.x = ur.x;
		lr.z = ll.z;
		lr.y = ll.y; // all y equal
	}

	RGB3f lineColor = {0.2, 0.2, 0.2};
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1,-1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((GLfloat)8);
	drawSquare(lr, ur, ul, ll, lineColor);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawSquare(lr, ur, ul, ll, color);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void drawCube() {
	Vec3f centerCoord = {0,0,0};
	double halfWidth = cubeWidth/2;
	if (myCube.position == 0 && cubeDebugInfo) {
		printf("Drawing cube 0 with rotation: {%i, %i, %i}",
			myCube.rotation.x,
			myCube.rotation.y,
			myCube.rotation.z
		);
	}

	glPushMatrix();
	glTranslatef(centerCoord.x, centerCoord.y, centerCoord.z);
	// glScalef( 2.0, 2.0, 0.0 ); use this instead of adding/subtracting halfWidth

	// Rotate to draw cube
	glRotatef( myCube.rotation.x, 1.0, 0.0, 0.0 );
	glRotatef( myCube.rotation.y, 0.0, 1.0, 0.0 );
	glRotatef( myCube.rotation.z, 0.0, 0.0, 1.0 );

	// FRONT
	Vec3f ur = {origin.x+halfWidth, origin.y+halfWidth, origin.z-halfWidth};
	Vec3f ll = {origin.x-halfWidth, origin.y-halfWidth, origin.z-halfWidth};
	drawRectangle(ll, ur, myCube.front.color);

	// BACK
	ur = (Vec3f){ .x = origin.x+halfWidth, .y = origin.y+halfWidth, .z = origin.z+halfWidth};
	ll = (Vec3f){ .x = origin.x-halfWidth, .y = origin.y-halfWidth, .z = origin.z+halfWidth};
	drawRectangle(ll, ur, myCube.back.color);

	// RIGHT
	ur = (Vec3f){ .x = origin.x+halfWidth, .y = origin.y+halfWidth, .z = origin.z+halfWidth };
	ll = (Vec3f){ .x = origin.x+halfWidth, .y = origin.y-halfWidth, .z = origin.z-halfWidth };
	drawRectangle(ll, ur, myCube.right.color);

	// LEFT
	ur = (Vec3f){ .x = origin.x-halfWidth, .y = origin.y+halfWidth, .z = origin.z-halfWidth };
	ll = (Vec3f){ .x = origin.x-halfWidth, .y = origin.y-halfWidth, .z = origin.z+halfWidth };
	drawRectangle(ll, ur, myCube.left.color);

	// TOP
	ur = (Vec3f){ .x = origin.x+halfWidth, .y = origin.y+halfWidth, .z = origin.z+halfWidth };
	ll = (Vec3f){ .x = origin.x-halfWidth, .y = origin.y+halfWidth, .z = origin.z-halfWidth };
	drawRectangle(ll, ur, myCube.top.color);

	// BOTTOM
	ur = (Vec3f){ .x = origin.x+halfWidth, .y = origin.y-halfWidth, .z = origin.z+halfWidth };
	ll = (Vec3f){ .x = origin.x-halfWidth, .y = origin.y-halfWidth, .z = origin.z-halfWidth };
	drawRectangle(ll, ur, myCube.bottom.color);

	glPopMatrix();
}

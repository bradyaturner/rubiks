#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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
void keyboardHandler(unsigned char key, int x, int y);
void resetDebugInfo();
void resetRotation();

double rotate_y=0; 
double rotate_x=0;

double cubeWidth = 0.3;

int windowId;

Cube myCube;
int printed = 0;
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

	glFlush();
	glutSwapBuffers();
}

void resetDebugInfo() {
	printed = 0;
	printf("Cube at position: %i, rotation: (%i, %i, %i)\n",
		myCube.position, myCube.rotation.x, myCube.rotation.y, myCube.rotation.z
	);
}

void specialKeys( int key, int x, int y ) {
	if (key == GLUT_KEY_RIGHT) {
		rotate_y += 5;
	} else if (key == GLUT_KEY_LEFT) {
		rotate_y -= 5;
	} else if (key == GLUT_KEY_UP) {
		rotate_x += 5;
	} else if (key == GLUT_KEY_DOWN) {
		rotate_x -= 5;
	}

	glutPostRedisplay();
}

void keyboardHandler(unsigned char key, int x, int y) {
	printf("Key pressed: %i\n", key);
	switch (key)
	{
		case 27:
		case 81:
		case 113:
			glutDestroyWindow(windowId); // should I check if this is initialized?
			exit(0);
			break;
		case 112:
			resetDebugInfo();
			break;
		case 33:
			rotateF(&myCube, 1, -1); // top layer ccw
			break;
		case 35:
			rotateF(&myCube, 5, -1); // front layer cw
			break;
		case 36:
			rotateF(&myCube, 6, -1); // SHIFT-4 back layer ccw
			break;
		case 37:
			rotateF(&myCube, 4, -1); // SHIFT-5 right layer ccw
			break;
		case 48:
			resetRotation();
			break;
		case 49:
			rotateF(&myCube, 1, 1); // top layer cw
			break;
		case 50:
			rotateF(&myCube, 2, 1); // bottom layer cw
			break;
		case 51:
			rotateF(&myCube, 5, 1); // front layer cw
			break;
		case 52:
			rotateF(&myCube, 6, 1); // 4 back layer cw
			break;
		case 53:
			rotateF(&myCube, 4, 1); // 5 right layer cw
			break;
		case 54:
			rotateF(&myCube, 3, 1); // 6 left layer cw
			break;
		case 64:
			rotateF(&myCube, 2, -1); // bottom layer ccw
			break;
		case 94:
			rotateF(&myCube, 3, -1); // SHIFT-6 left layer ccw
			break;
	}
	glutPostRedisplay();
}

void resetRotation() {
	rotate_x = 0;
	rotate_y = 0;
}

void initialize() {
	initializeCube(&myCube);
}

int main( int argc, char* argv[] ){
	initialize();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	windowId = glutCreateWindow("Rubik's Cube");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc(keyboardHandler);
	glClearColor(0.3, 0.3, 0.3, 0.0);

	glutMainLoop();

	return 0;
}

void drawSquare(Vec3f lr, Vec3f ur, Vec3f ul, Vec3f ll, RGB3f color) {
	glBegin(GL_POLYGON);
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

	drawSquare(lr, ur, ul, ll, color);
}

void drawCube() {
	Vec3f centerCoord = {0,0,0};
	double halfWidth = cubeWidth/2;

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

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "cube.h"

typedef struct {
	float x;
	float y;
	float z;
} Vec3f;

typedef struct{
    Vec3f ll;
    Vec3f ur;
} Rectangle;

void initialize();
void display();
void drawRubiksCube();
void drawCube(Cube cube, Vec3f centerCoord);
void drawRectangle(Vec3f ll, Vec3f ur, RGB3f color);
void specialKeys( int key, int x, int y );
void keyboardHandler(unsigned char key, int x, int y);

double rotate_y=0; 
double rotate_x=0;

double cubeWidth = 0.3;

int windowId;

Rubiks rubiksCube;

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Other Transformations
	// glTranslatef( 0.1, 0.0, 0.0 );      // Not included
	// glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

	// Rotate when user changes rotate_x and rotate_y
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );

	// Other Transformations
	// glScalef( 2.0, 2.0, 0.0 );          // Not included

	drawRubiksCube();

	glFlush();
	glutSwapBuffers();
}

void drawRubiksCube(){
	// TODO there has to be a smarter way to do this math
	int i = 0;
	for(int x=-1; x<=1; x++){
		for(int y=-1; y<=1; y++){
			for(int z=-1; z<=1; z++){
				Vec3f cubeCoord = {x*cubeWidth, y*cubeWidth, z*cubeWidth};
				int cubeNum = findCube(&rubiksCube, i);
				drawCube(rubiksCube.cubes[cubeNum], cubeCoord);
				i++;
			}
		}
	}
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
	switch (key)
	{
		case 27:
		case 81:
		case 113:
			glutDestroyWindow(windowId); // should I check if this is initialized?
			exit(0);
			break;
		case 49:
			rotateCubeFace(&rubiksCube, 1, 1);
			break;
	}
	glutPostRedisplay();
}

void initialize() {
	initializeRubiks(&rubiksCube);
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

void drawCube(Cube cube, Vec3f centerCoord) {
	double halfWidth = cubeWidth/2;

	// TODO add support for specifying cube face colors
	//RGB3f color = {1.0, 1.0, 1.0};

	// TODO cube face orientations aren't correct -- need to normalize them relative to cube's orientation

	// FRONT
	Vec3f ur = {centerCoord.x+halfWidth, centerCoord.y+halfWidth, centerCoord.z-halfWidth};
	Vec3f ll = {centerCoord.x-halfWidth, centerCoord.y-halfWidth, centerCoord.z-halfWidth};
	drawRectangle(ll, ur, cube.front.color);

	// BACK
	ur = (Vec3f){ .x = centerCoord.x+halfWidth, .y = centerCoord.y+halfWidth, .z = centerCoord.z+halfWidth};
	ll = (Vec3f){ .x = centerCoord.x-halfWidth, .y = centerCoord.y-halfWidth, .z = centerCoord.z+halfWidth};
	drawRectangle(ll, ur, cube.back.color);

	// RIGHT
	ur = (Vec3f){ .x = centerCoord.x+halfWidth, .y = centerCoord.y+halfWidth, .z = centerCoord.z+halfWidth };
	ll = (Vec3f){ .x = centerCoord.x+halfWidth, .y = centerCoord.y-halfWidth, .z = centerCoord.z-halfWidth };
	drawRectangle(ll, ur, cube.right.color);

	// LEFT
	ur = (Vec3f){ .x = centerCoord.x-halfWidth, .y = centerCoord.y+halfWidth, .z = centerCoord.z-halfWidth };
	ll = (Vec3f){ .x = centerCoord.x-halfWidth, .y = centerCoord.y-halfWidth, .z = centerCoord.z+halfWidth };
	drawRectangle(ll, ur, cube.left.color);

	// TOP
	ur = (Vec3f){ .x = centerCoord.x+halfWidth, .y = centerCoord.y+halfWidth, .z = centerCoord.z+halfWidth };
	ll = (Vec3f){ .x = centerCoord.x-halfWidth, .y = centerCoord.y+halfWidth, .z = centerCoord.z-halfWidth };
	drawRectangle(ll, ur, cube.top.color);

	// BOTTOM
	ur = (Vec3f){ .x = centerCoord.x+halfWidth, .y = centerCoord.y-halfWidth, .z = centerCoord.z+halfWidth };
	ll = (Vec3f){ .x = centerCoord.x-halfWidth, .y = centerCoord.y-halfWidth, .z = centerCoord.z-halfWidth };
	drawRectangle(ll, ur, cube.bottom.color);
}

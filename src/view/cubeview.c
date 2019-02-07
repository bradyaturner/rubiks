#include "view/cubeview.h"
#include "view/constants.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGl/gl.h>
#else
#include <GL/gl.h>
#endif

static GLfloat vertices[] =
{
	-0.5, -0.5, -0.5,		-0.5, -0.5, 0.5,		-0.5, 0.5, 0.5,		-0.5, 0.5, -0.5, // left
	0.5, -0.5, -0.5,		0.5, -0.5,  0.5,		0.5,  0.5, 0.5,		0.5,  0.5, -0.5, // right
	-0.5, -0.5, -0.5,		-0.5, -0.5,  0.5,		0.5, -0.5, 0.5,		0.5, -0.5, -0.5, // bottom
	-0.5,  0.5, -0.5,		-0.5,  0.5,  0.5,		0.5, 0.5, 0.5,		0.5,  0.5, -0.5, // top
	-0.5, -0.5, -0.5,		-0.5,  0.5, -0.5,		0.5, 0.5, -0.5,		0.5, -0.5, -0.5, // front
	-0.5, -0.5,  0.5,		-0.5,  0.5,  0.5,		0.5, 0.5, 0.5,		0.5, -0.5,  0.5  // back
};

static void cube_drawNormalized(const Cube cube, int useColor);

static const float scale = 0.95;
static const float lineWidth = 5.0; // this should be proportional to cube size
void cube_draw(Cube cube, Vec3f coords, RGB3f lineColor, double cubeWidth) {
	glPushMatrix();

	glTranslatef(coords.x, coords.y, coords.z);
	glScalef(cubeWidth*scale, cubeWidth*scale, cubeWidth*scale);
	glMultMatrixf(quat_toMatrix(&cube.quat));

	// Draw outline
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(0,-1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((GLfloat)lineWidth);
	glColor3f(lineColor.red, lineColor.green, lineColor.blue); // line color
	cube_drawNormalized(cube, 0);
	glDisable(GL_POLYGON_OFFSET_LINE);

	// Draw solid polygons
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1); // just guessing on these values
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cube_drawNormalized(cube, 1);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

static void cube_drawNormalized(const Cube cube, int useColor) {
	glEnableClientState(GL_VERTEX_ARRAY);
	if (useColor)
		glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if (useColor) {
		float *colors = cube_getColorArray(cube);
		glColorPointer(3, GL_FLOAT, 0, colors);
	}

	glDrawArrays(GL_QUADS, 0, 24);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (useColor)
		glDisableClientState(GL_COLOR_ARRAY);
}

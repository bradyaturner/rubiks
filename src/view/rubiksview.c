#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGl/gl.h>
#else
#include <GL/gl.h>
#endif

#include "view/rubiksview.h"
#include "view/cubeview.h"
#include "view/constants.h"
#include "controller/rubikscontroller.h"
#include "vector.h"
#include "logger.h"

void rc_getFaceRotation(Cube *cube);

void rc_draw(Rubiks *rubiks){
	for(int i=0; i<NUM_CUBES; i++){
		Cube *cube = rc_getCubeAtPos(rubiks, i);
		Vec3f cubeCoord = vec3fMultiplyScalar(rc_determineCubeCoord(cube), cubeWidth);
		glPushMatrix();
		rc_getFaceRotation(cube);
		if (cube->id == rubiks->cubeInProgress) {
			cube_draw(*cube, cubeCoord, inProgressLineColor, cubeWidth);
		} else {
			cube_draw(*cube, cubeCoord, lineColor, cubeWidth);
		}
		glPopMatrix();
	}
}

void rc_getFaceRotation(Cube *cube) {
	// assuming that only one face at a time will rotate
	for (int i=0; i<NUM_FACES; i++) {
		if (rc_faceIsRotating(i)) {
			if (rc_checkCubeInFace(cube, i)) {
				glRotatef( rc_getFaceRotationDegrees(i), faceData[i].normal.x, faceData[i].normal.y, faceData[i].normal.z );
				break;
			}
		}
	}
}

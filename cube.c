#include "cube.h"
#include <stdio.h>

void rotateCube(Cube *cube, const Vec3i degrees) {
	printf("Cube rotation before: %i, %i, %i...", cube->rotation.x, cube->rotation.y, cube->rotation.z);
	cube->rotation = vectorAdd(cube->rotation, degrees);
	cube->rotation.x = cube->rotation.x % 360;
	cube->rotation.y = cube->rotation.y % 360;
	cube->rotation.z = cube->rotation.z % 360;
	printf("Cube rotation after: %i, %i, %i\n", cube->rotation.x, cube->rotation.y, cube->rotation.z);
}


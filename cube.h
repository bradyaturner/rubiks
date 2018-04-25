typedef struct {
	float red;
	float green;
	float blue;
} RGB3f;


typedef struct {
	RGB3f color;
} CubeFace;

typedef struct {
	CubeFace top;
	CubeFace bottom;
	CubeFace left;
	CubeFace right;
	CubeFace front;
	CubeFace back;
	int num;
} Cube;

typedef struct {
	Cube cubes[27];
} Rubiks;

int findCube(Rubiks *rubiks, int cubePosition) {
	int foundIndex = -1;
	for (int i=0; i<27; i++) {
		if (rubiks->cubes[i].num == cubePosition) {
			foundIndex = i;
			break;
		}
	}
	return foundIndex;
}

void initializeRubiks(Rubiks *rubiks) {
	for (int i = 0; i<27; i++) {
		rubiks->cubes[i].num = i;
		rubiks->cubes[i].top.color = (RGB3f){1.0, 1.0, 1.0}; // white
		rubiks->cubes[i].bottom.color = (RGB3f){1.0, 1.0, 0.0}; // yellow
		rubiks->cubes[i].left.color = (RGB3f){1.0, 0.0, 0.0}; // red
		rubiks->cubes[i].right.color = (RGB3f){1.0, 0.65, 0.0}; // orange
		rubiks->cubes[i].front.color = (RGB3f){0.0, 1.0, 0.0}; // green
		rubiks->cubes[i].back.color = (RGB3f){0.0, 0.0, 1.0}; // blue
	}
}

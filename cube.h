typedef struct {
	float red;
	float green;
	float blue;
} RGB3f;

typedef struct {
    int x;
    int y;
    int z;
} Vec3i;

Vec3i vectorAdd(Vec3i n1, Vec3i n2) {
	Vec3i tmp;
	tmp.x = n1.x + n2.x;
	tmp.y = n1.y + n2.y;
	tmp.z = n1.z + n2.z;
	return tmp;
}

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
	int position;
	Vec3i rotation; // degrees of rotation about each axis, can be +/- but should be multiples of 90
} Cube;

typedef struct {
	Cube cubes[27];
} Rubiks;

const int topLayer[9]			= {0, 1, 2, 3, 4, 5, 6, 7, 8};
const int topLayerRotation[9]	= {2, 5, 8, 1, 4, 7, 0, 3, 6}; // to rotate ccw flip layer and rotation values
const Vec3i topLayerCWDegrees = (Vec3i){0, 90, 0};
const Vec3i topLayerCCWDegrees = (Vec3i){0, -90, 0};

const int bottomLayer[9]			= {18, 19, 20, 21, 22, 23, 24, 25, 26};
const int bottomLayerRotation[9]	= {24, 21, 18, 25, 22, 19, 26, 23, 20};
const Vec3i bottomLayerCWDegrees = (Vec3i){0, -90, 0};
const Vec3i bottomLayerCCWDegrees = (Vec3i){0, 90, 0};

const int frontLayer[9]			= {6, 7, 8, 15, 16, 17, 24, 25, 26};
const int frontLayerRotation[9]	= {8, 17, 26, 7, 16, 25, 6, 15, 24};
const Vec3i frontLayerCWDegrees = (Vec3i){0, 0, 90};
const Vec3i frontLayerCCWDegrees = (Vec3i){0, 0, -90};

void initializeRubiks(Rubiks *rubiks);
void rotateCubeFace(Rubiks *rubiks, int face, int direction);
int findCube(Rubiks *rubiks, int cubePosition);
void rotateLayer(Rubiks *rubiks, const int layer[], const int rotation[], const Vec3i degrees);

void rotateLayer(Rubiks *rubiks, const int layer[], const int rotation[], const Vec3i degrees) {
	// get index of cubes actually occupying top layer positions
	// should end up with 9 values
	// for each one, update its position to be its rotation value
	int indices[9];
	for (int i=0; i<9; i++) {
		printf("Looking for cube at position = %i", layer[i]);
		indices[i] = findCube(rubiks, layer[i]);
		printf("\tFound cube index: %i\n", indices[i]);
	}
	for (int i=0; i<9; i++) {
		printf("Rotating cube at postition %i to position %i\n",
			rubiks->cubes[indices[i]].position,
			rotation[i]
		);
		rubiks->cubes[indices[i]].position = rotation[i];
		Vec3i curRotation = rubiks->cubes[indices[i]].rotation;
		printf("Cube rotation before: %i, %i, %i...", curRotation.x, curRotation.y, curRotation.z);
		rubiks->cubes[indices[i]].rotation = vectorAdd(rubiks->cubes[indices[i]].rotation, degrees);
		rubiks->cubes[indices[i]].rotation.x = rubiks->cubes[indices[i]].rotation.x % 360;
		rubiks->cubes[indices[i]].rotation.y = rubiks->cubes[indices[i]].rotation.y % 360;
		rubiks->cubes[indices[i]].rotation.z = rubiks->cubes[indices[i]].rotation.z % 360;
		Vec3i curRotation2 = rubiks->cubes[indices[i]].rotation;
		printf("Cube rotation after: %i, %i, %i\n", curRotation2.x, curRotation2.y, curRotation2.z);
	}
}

void rotateCubeFace(Rubiks *rubiks, int face, int direction) { // positive anything=cw, negative anything=ccw
	// once I start modifying cubes, looking them up by their number won't work anymore...
	// each rotation can be defined as 8 moves (or the reverse of that if ccw
	// can define a single method that takes those 8 moves and looks up 8 indices and calculates new positions
	// CONVENTION: looking from positive to negative end of axis and rotating cw gives you +deg rotation
	// so rotating the top layer clockwise is +deg, but rotating the bottom layer clockwise is -deg
	switch(face)
	{
		case 1: // top -- rotation about y axis (cw is +deg)
		{
			if (direction >= 0) {
				rotateLayer(rubiks, topLayer, topLayerRotation, topLayerCWDegrees);
			} else {
				rotateLayer(rubiks, topLayerRotation, topLayer, topLayerCCWDegrees);
			}
			break;
		}
		case 2: // bottom -- rotation about y axis
			if (direction >= 0) {
				rotateLayer(rubiks, bottomLayer, bottomLayerRotation, bottomLayerCWDegrees);
			} else {
				rotateLayer(rubiks, bottomLayerRotation, bottomLayer, bottomLayerCCWDegrees);
			}
			break;
		case 3: // left -- rotation about x axis
		case 4: // right -- rotation about x axis
		case 5: // front
			if (direction >= 0) {
				rotateLayer(rubiks, frontLayer, frontLayerRotation, frontLayerCWDegrees);
			} else {
				rotateLayer(rubiks, frontLayerRotation, frontLayer, frontLayerCCWDegrees);
			}
			break;
		case 6: // back
		default:
			break;
	}
}

int findCube(Rubiks *rubiks, int cubePosition) {
	int foundIndex = -1;
	for (int i=0; i<27; i++) {
		if (rubiks->cubes[i].position == cubePosition) {
			foundIndex = i;
			break;
		}
	}
	return foundIndex;
}

void initializeRubiks(Rubiks *rubiks) {
	for (int i = 0; i<27; i++) {
		rubiks->cubes[i].position = i;
		rubiks->cubes[i].rotation = (Vec3i){0, 0, 0};
		rubiks->cubes[i].top.color = (RGB3f){1.0, 1.0, 1.0}; // white
		rubiks->cubes[i].bottom.color = (RGB3f){1.0, 1.0, 0.0}; // yellow
		rubiks->cubes[i].left.color = (RGB3f){1.0, 0.0, 0.0}; // red
		rubiks->cubes[i].right.color = (RGB3f){1.0, 0.65, 0.0}; // orange
		rubiks->cubes[i].front.color = (RGB3f){0.0, 1.0, 0.0}; // green
		rubiks->cubes[i].back.color = (RGB3f){0.0, 0.0, 1.0}; // blue
	}
}

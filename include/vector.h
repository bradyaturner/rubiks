#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    int x;
    int y;
    int z;
} Vec3i;

typedef struct {
	float x;
	float y;
	float z;
} Vec3f;

Vec3i vectorAdd(Vec3i n1, Vec3i n2);
Vec3i vectorMultiply(Vec3i n1, int c);
Vec3f vec3fNormalize(Vec3f vec);
int vec3fCompare(Vec3f a, Vec3f b);
#endif

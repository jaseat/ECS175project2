#pragma once
#ifndef TRANS_H
#define TRANS_H
#include <math.h>

class wcPt3d
{
public:
	float x, y, z;
};
typedef float Matrix4x4[4][4];

extern Matrix4x4 matComposite;
void matrix4x4SetIdentity(Matrix4x4 matIdent4x4);
void matrix4x4PreMultiply(Matrix4x4 m1, Matrix4x4 m2);
void translate3D(float tx, float ty, float tz);
void rotate3D(wcPt3d p1, wcPt3d p2, float radianAngle);
void scale3D(float sx, float sy, float sz, wcPt3d fixedPt);
#endif
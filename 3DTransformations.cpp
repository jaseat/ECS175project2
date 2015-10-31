#include "3DTransformations.h"

Matrix4x4 matComposite;

void matrix4x4SetIdentity(Matrix4x4 matIdent4x4)
{
	int row, col;
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matIdent4x4[row][col] = (row == col);
}

void matrix4x4PreMultiply(Matrix4x4 m1, Matrix4x4 m2)
{
	int row, col;
	Matrix4x4 matTemp;

	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matTemp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] * m2[1][col] + m1[row][2] * m2[2][col] + m1[row][3] * m2[3][col];
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			m2[row][col] = matTemp[row][col];
}

void translate3D(float tx, float ty, float tz)
{
	Matrix4x4 matTransl3D;

	matrix4x4SetIdentity(matTransl3D);

	matTransl3D[0][3] = tx;
	matTransl3D[1][3] = ty;
	matTransl3D[2][3] = tz;

	matrix4x4PreMultiply(matTransl3D, matComposite);
}

void rotate3D(wcPt3d p1, wcPt3d p2, float radianAngle)
{
	Matrix4x4 matQuatRot;

	float axisVectLength = sqrt((p2.x - p1.x) * (p2.x - p1.x) +
		(p2.y - p2.x) * (p2.y - p2.x) +
		(p2.z - p1.z) * (p2.z - p1.z));

	float cosA = cosf(radianAngle);
	float oneC = 1 - cosA;
	float sinA = sinf(radianAngle);
	float ux = (p2.x - p1.x) / axisVectLength;
	float uy = (p2.y - p1.y) / axisVectLength;
	float uz = (p2.z - p1.z) / axisVectLength;

	translate3D(-p1.x, -p1.y, -p1.z);

	matrix4x4SetIdentity(matQuatRot);

	matQuatRot[0][0] = ux*ux*oneC + cosA;
	matQuatRot[0][1] = ux*uy*oneC - uz*sinA;
	matQuatRot[0][2] = ux*uz*oneC + uy*sinA;
	matQuatRot[1][0] = uy*ux*oneC + uz*sinA;
	matQuatRot[1][1] = uy*uy*oneC + cosA;
	matQuatRot[1][2] = uy*uz*oneC - ux*sinA;
	matQuatRot[2][0] = uz*ux*oneC - uy*sinA;
	matQuatRot[2][1] = uz*uy*oneC + ux*sinA;
	matQuatRot[2][2] = uz*uz*oneC + cosA;

	matrix4x4PreMultiply(matQuatRot, matComposite);

	translate3D(p1.x, p1.y, p1.z);
}

void scale3D(float sx, float sy, float sz, wcPt3d fixedPt)
{
	Matrix4x4 matScale3D;

	matrix4x4SetIdentity(matScale3D);

	matScale3D[0][0] = sx;
	matScale3D[0][3] = (1 - sx) * fixedPt.x;
	matScale3D[1][1] = sy;
	matScale3D[1][3] = (1 - sy) * fixedPt.y;
	matScale3D[2][2] = sz;
	matScale3D[2][3] = (1 - sz) * fixedPt.z;

	matrix4x4PreMultiply(matScale3D, matComposite);
}

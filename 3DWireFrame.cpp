#include "3DWireFrame.h"


WireFrame::WireFrame(std::vector<wcPt3d> listOfPoints, std::vector<edge> listOfEdges, float* Buffer1, float* Buffer2, float* Buffer3, int width, int height)
{
	this->listOfPoints = listOfPoints;
	this->listOfEdges = listOfEdges;
	this->Buffer1 = Buffer1;
	this->Buffer2 = Buffer2;
	this->Buffer3 = Buffer3;
	this->width = width;
	this->height = height;
	calculatedCentroid(); calculatedCentroid();
	red = 1.0;
	green = 1.0;
	blue = 1.0;
}

WireFrame::WireFrame(std::vector<wcPt3d> listOfPoints, std::vector<edge> listOfEdges, float* Buffer1, float* Buffer2, float* Buffer3, int width, int height, float red, float green, float blue)
{
	this->listOfPoints = listOfPoints;
	this->listOfEdges = listOfEdges;
	this->Buffer1 = Buffer1;
	this->Buffer2 = Buffer2;
	this->Buffer3 = Buffer3;
	this->width = width;
	this->height = height;
	calculatedCentroid(); calculatedCentroid();
	this->red = red;
	this->green = green;
	this->blue = blue;
}

void WireFrame::transformPoint()
{
	std::vector<wcPt3d> tempList;
	wcPt3d tempPoint;

	for (unsigned int i = 0; i < listOfPoints.size(); i++)
	{
		wcPt3d curP = listOfPoints[i];
		tempPoint.x = matComposite[0][0] * curP.x + matComposite[0][1] * curP.y + matComposite[0][2] * curP.z + matComposite[0][3] * 1;
		tempPoint.y = matComposite[1][0] * curP.x + matComposite[1][1] * curP.y + matComposite[1][2] * curP.z + matComposite[1][3] * 1;
		tempPoint.z = matComposite[2][0] * curP.x + matComposite[2][1] * curP.y + matComposite[2][2] * curP.z + matComposite[2][3] * 1;
		tempList.push_back(tempPoint);
	}

	listOfPoints = tempList;
}

void WireFrame::translate(float tx, float ty, float tz)
{
	matrix4x4SetIdentity(matComposite);
	translate3D(tx, ty, tz);
	transformPoint();
	calculatedCentroid();
}

void WireFrame::rotate(wcPt3d p1, wcPt3d p2, float radianAngle)
{
	matrix4x4SetIdentity(matComposite);
	rotate3D(p1, p2, radianAngle);
	transformPoint();
	calculatedCentroid();
}

void WireFrame::scale(float sx, float sy, float sz)
{
	matrix4x4SetIdentity(matComposite);
	scale3D(sx, sy, sz, centroid);
	transformPoint();
	calculatedCentroid();
}

void WireFrame::draw()
{
	int size = listOfEdges.size();
	
	for (int i = 0; i < size; i++)
	{
		wcPt3d begin = normalizedPoints[listOfEdges[i].begin - 1], end = normalizedPoints[listOfEdges[i].end - 1];
		//x-y projection
		DDADraw(begin.x*width / 2, begin.y*height / 2, end.x*width / 2, end.y*height / 2, width / 2, height / 2, Buffer1, red, green, blue);
		//y-z projection
		DDADraw(begin.y*width / 2, begin.z*height / 2, end.y*width / 2, end.z*height / 2, width / 2, height / 2, Buffer2, red, green, blue);
		//x-z projection
		DDADraw(begin.x*width / 2, begin.z*height / 2, end.x*width / 2, end.z*height / 2, width / 2, height / 2, Buffer3, red, green, blue);
	}
}

void WireFrame::calculatedCentroid()
{
	int size = listOfPoints.size();
	float xavg = 0, yavg = 0, zavg = 0;
	for (int i = 0; i < size; i++)
	{
		xavg += listOfPoints[i].x;
		yavg += listOfPoints[i].y;
		zavg += listOfPoints[i].z;
	}
	xavg = xavg / size;
	yavg = yavg / size;
	zavg = zavg / size;
	centroid.x = xavg;
	centroid.y = yavg;
	centroid.z = zavg;
}

wcPt3d WireFrame::getPoint(int index)
{
	return listOfPoints[index];
}

void WireFrame::getMinMax(wcPt3d * minPoint, wcPt3d * maxPoint)
{
	wcPt3d curP;
	int size = listOfPoints.size();
	for (int i = 0; i < size; i++)
	{
		curP = listOfPoints[i];
		if (curP.x < minPoint->x)
			minPoint->x = curP.x;
		if (curP.y < minPoint->y)
			minPoint->y = curP.y;
		if (curP.z < minPoint->z)
			minPoint->z = curP.z;
		if (curP.x > maxPoint->x)
			maxPoint->x = curP.x;
		if (curP.y > maxPoint->y)
			maxPoint->y = curP.y;
		if (curP.z > maxPoint->z)
			maxPoint->z = curP.z;
	}
}



void WireFrame::normalizePoints(float masterMin, float masterMax)
{
	float minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	int size = listOfPoints.size();
	wcPt3d tempP;
	float diff = masterMax - masterMin;

	normalizedPoints.clear();

	for (int i = 0; i < size; i++)
	{
		wcPt3d curP = listOfPoints[i];
		tempP.x = (curP.x - masterMin) / (diff);
		tempP.y = (curP.y - masterMin) / (diff);
		tempP.z = (curP.z - masterMin) / (diff);
		normalizedPoints.push_back(tempP);
	}
}

std::vector<wcPt3d> WireFrame::getPoints()
{
	return listOfPoints;
}

std::vector<edge> WireFrame::getEdges()
{
	return listOfEdges;
}

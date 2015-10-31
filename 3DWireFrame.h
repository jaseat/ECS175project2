#pragma once
#ifndef h3DWIREFRAME_H
#define h3DWIREFRAME_H
#include "Shape.h"
#include <vector>
#include "GraphicsProc.h"
#include "3DTransformations.h"

class edge
{
public:
	int begin;
	int end;
};

class WireFrame : public Shape
{
public:
	WireFrame() {}
	WireFrame(std::vector<wcPt3d> listofPoints, std::vector<edge> listOfEdges, float* Buffer1, float* Buffer2, float* Buffer3, int width, int height);
	WireFrame::WireFrame(std::vector<wcPt3d> listOfPoints, std::vector<edge> listOfEdges, float* Buffer1, float* Buffer2, float* Buffer3, int width, int height, float red, float green, float blue);
	void transformPoint();
	void translate(float tx, float ty, float tz);
	void rotate(wcPt3d p1, wcPt3d p2, float radianAngle);
	void scale(float sx, float sy, float sz);
	void draw();
	void calculatedCentroid();
	wcPt3d getPoint(int index);
	void getMinMax(wcPt3d* minPoint, wcPt3d* maxPoint);
	void normalizePoints(float masterMin, float masterMax);
	void clip(Point winMin, Point winMax){}
	std::vector<wcPt3d> getPoints();
	std::vector<edge> getEdges();
private:
	std::vector<wcPt3d> listOfPoints;
	std::vector<wcPt3d> normalizedPoints;
	std::vector<edge> listOfEdges;
	wcPt3d centroid;
	float *Buffer1, *Buffer2, *Buffer3;
	int width, height;
	float red, blue, green;
};
#endif
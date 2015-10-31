#pragma once
#ifndef POLYGON_H
#define POLYGON_H
#include <vector>
#include "GraphicsProc.h"
#include "Shape.h"
#include <GL/glut.h>


class myPolygon : public Shape
{
public:
	myPolygon();
	myPolygon(std::vector<Point>, float* PixelBuffer, int width, int height);
	void draw();
	void translate(float tx, float ty);
	void rotate(double theta);
	void scale(float sx, float sy);
	void calculatedCentroid();
	void clip(Point winMin, Point winMax);
	std::vector<Point> getList() { return lOP; }
private:
	std::vector<Point> listOfPoints;
	std::vector<Point> lOP;
	int width, height;
	float *PixelBuffer;
	Point centroid;
	Point winMin, winMax;
};
#endif

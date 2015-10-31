#pragma once
#ifndef LINE_H
#define LINE_H
#include "Shape.h"
#include "GraphicsProc.h"
class Line: public Shape
{
public:
	Line(int x0, int y0, int xEnd, int yEnd, int width, int height, float * PixelBuffer, int mode);
	void draw();
	void clip(Point winMin, Point winMax);
private:
	int x0, y0, xEnd, yEnd, height, width, mode;
	int x0Plot, y0Plot, xEndPlot, yEndPlot;
	float * PixelBuffer;
	Point winMin, winMax;
};
#endif

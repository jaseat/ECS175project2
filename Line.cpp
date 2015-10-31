#include "Line.h"
#include "GraphicsProc.h"

Line::Line(int x0, int y0, int xEnd, int yEnd, int width, int height, float * PixelBuffer, int mode)
{
	this->x0 = x0;
	this->y0 = y0;
	this->xEnd = xEnd;
	this->yEnd = yEnd;
	this->width = width;
	this->height = height;
	this->PixelBuffer = PixelBuffer;
	this->mode = mode;
	x0Plot = x0;
	y0Plot = y0;
	xEndPlot = xEnd;
	yEndPlot = yEnd;
	winMin.x = 0;
	winMin.y = 0;
	winMax.x = width;
	winMax.y = height;
	this->clip(winMin, winMax);
}

void Line::draw()
{
	if (mode == 0)
	{
		DDADraw(x0Plot, y0Plot, xEndPlot, yEndPlot, width, height, PixelBuffer);
	}
	else
		BresenhamDraw(x0Plot, y0Plot, xEndPlot, yEndPlot, width, height, PixelBuffer);
}

void Line::clip(Point winMin, Point winMax)
{
	this->winMin = winMin;
	this->winMax = winMax;
	Point p1, p2;
	p1.x = x0;
	p1.y = y0;
	p2.x = xEnd;
	p2.y = yEnd;
	this->winMin = winMin;
	this->winMax = winMax;

	lineClipCohStuth(winMin, winMax, p1, p2);

	x0Plot = p1.x;
	y0Plot = p1.y;
	xEndPlot = p2.x;
	yEndPlot = p2.y;
}

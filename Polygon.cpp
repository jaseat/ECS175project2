#include "Polygon.h"
#include <iostream>
#include <math.h>

myPolygon::myPolygon()
{
}

myPolygon::myPolygon(std::vector<Point> pList, float* PixelBuffer, int width, int height)
{
	lOP = pList;
	this->PixelBuffer = PixelBuffer;
	this->width = width;
	this->height = height;

	this->calculatedCentroid();

	winMin.x = 1;
	winMin.y = 1;
	winMax.x = width-1;
	winMax.y = height-1;
	this->clip(winMin, winMax);
}

void myPolygon::draw()
{
	int length = listOfPoints.size();
	bool isIn = false;
	float *TempBuffer; // this will create a tempory copy of the "screen" where the edges of the polygon will be created for the scanline algorithm
	TempBuffer = new float[height*width * 3];
	for (int i = 0; i < length - 1; i++) {
		DDADraw(listOfPoints[i].x, listOfPoints[i].y, listOfPoints[i + 1].x, listOfPoints[i + 1].y, width, height, TempBuffer);
	}
	DDADraw(listOfPoints[length-1].x, listOfPoints[length-1].y, listOfPoints[0].x, listOfPoints[0].y, width, height, TempBuffer);

	//Perform scanline operation
	for (int y = 0; y < height; y++)
	{
		isIn = false;
		int rightEnd = width;
		for (int x = 0; x < rightEnd; x++)
		{
			if (isIn)
			{
				setPixel(PixelBuffer, x, y, width, height);
			}
			if (!isIn && getPixel(TempBuffer, x, y, width, height))
			{
					isIn ? isIn = false : isIn = true;
					//start scan from left end of the screen to find right end of boundry, only works on convex polygons
					for (int j = width; j > x-1; j--)
					{
						if (getPixel(TempBuffer, j, y, width, height))
						{
							rightEnd = j + 1 ;
							break;
						}
					}
				setPixel(PixelBuffer, x, y, width, height);
			}
		}
	}
}

void myPolygon::translate(float tx, float ty)
{
	int k;
	int size = lOP.size();

	for (k = 0; k < size; k++)
	{
		lOP[k].x = lOP[k].x + (int)tx;
		lOP[k].y = lOP[k].y + (int)ty;
	}

	this->calculatedCentroid();
}

void myPolygon::rotate(double theta)
{
	int size = lOP.size();
	for (int k = 0; k < size; k++)
	{
		Point p = lOP[k];
		lOP[k].x = centroid.x + (p.x - centroid.x) * cos(theta)
			- (p.y - centroid.y) * sin(theta);
		lOP[k].y = centroid.y + (p.x - centroid.x) * sin(theta)
			+ (p.y - centroid.y) * cos(theta);
	}

	this->calculatedCentroid();
}

void myPolygon::scale(float sx, float sy)
{
	int size = lOP.size();
	for (int k = 0; k < size; k++)
	{
		Point p = lOP[k];
		lOP[k].x = p.x * sx + centroid.x * (1 - sx);
		lOP[k].y = p.y * sy + centroid.y * (1 - sy);
	}
	this->calculatedCentroid();
}

void myPolygon::calculatedCentroid()
{
	int size = lOP.size();
	int xavg = 0, yavg = 0;
	for (int i = 0; i < size; i++)
	{
		xavg += lOP[i].x;
		yavg += lOP[i].y;
	}
	xavg = xavg / size;
	yavg = yavg / size;
	centroid.x = xavg;
	centroid.y = yavg;
}

void myPolygon::clip(Point winMin, Point winMax)
{
	this->winMin = winMin;
	this->winMax = winMax;
	int size = lOP.size();
	Point *points1 = new Point[size];
	Point *points2 = new Point[100];
	for (int i = 0; i < size; i++) {
		points1[i] = lOP[i];
	}
	listOfPoints.clear();
	int count = polygonClipSuthHodg(winMin, winMax, size, points1, points2);
	for (int i = 0; i < count; i++) {
		listOfPoints.push_back(points2[i]);
	}

	delete points1;
	delete points2;
}

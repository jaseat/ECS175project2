#include <math.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include "GraphicsProc.h"

inline int round(const float a) { return int(a + 0.5); }
inline void swap(int &a, int &b) { int t = a; a = b; b = t; }

const int winLeftBitCode = 0x1;
const int winRightBitCode = 0x2;
const int winBottomBitCode = 0x4;
const int winTopBitCode = 0x8;

inline int inside(int code) { return int(!code); }
inline int reject(int code1, int code2) { return int(code1 & code2); }
inline int accept(int code1, int code2) { return int(!(code1 | code2)); }

Boundary& operator++(Boundary& b, int)
{
	switch (b) {
	case Left: b = Right; return b;
	case Right: b = Bottom; return b;
	case Bottom: b = Top; return b;
	case Top: b = END_BOUNDARY; return b;
	}
}


const int nClip = 4;

//This function an x-coordinate, and a y-coordinate and sets that corrsepondign pixel in the PixelBuffer to on. For now this means setting the pixel to white (e.g. 1.0, 1.0, 1.0)
void setPixel(float* PixelBuffer, int x, int y, int width, int height, float red, float blue, float green)
{
	PixelBuffer[(x + width * y) * 3 + 0] = red;
	PixelBuffer[(x + width * y) * 3 + 1] = blue;
	PixelBuffer[(x + width * y) * 3 + 2] = green;
}

void setPixel(float* PixelBuffer, int x, int y, int width, int height)
{
	setPixel(PixelBuffer, x, y, width, height, 1, 1, 1);
}

//This function will return true if the pixel requested is set on
bool getPixel(float* PixelBuffer, int x, int y, int width, int height)
{
	if (PixelBuffer[(x + width * y) * 3 + 0] == 1.0)
		return true;
	else if (x > width || y > height)
		return false;
	else
		return false;
}

bool DDADraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer, float red, float green, float blue)
{
	int dx = xEnd - x0, dy = yEnd - y0, steps, k;
	float xIncrement, yIncrement, x = x0, y = y0;
	if (x0 < 0 || y0 < 0 || xEnd > width || yEnd > height)
		return false;

	if (fabs(dx) > fabs(dy))
		steps = fabs(dx);
	else
		steps = fabs(dy);
	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);

	setPixel(PixelBuffer, round(x), round(y), width, height, red, blue, green);
	for (k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;
		setPixel(PixelBuffer, round(x), round(y), width, height, red, blue, green);
	}
	return true;
}

bool DDADraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer)
{
	return DDADraw(x0, y0, xEnd, yEnd, width, height, PixelBuffer, 1.0, 1.0, 1.0);
}

bool BresenhamDraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer)
{
	if (x0 < 0 || y0 < 0 || xEnd > width || yEnd > height)
		return false;
	int dx = fabs(xEnd - x0), dy = fabs(yEnd - y0);
	bool isSwapped = false;
	if (dy / dx > 1) {
		swap(x0, y0);
		swap(xEnd, yEnd);
		swap(dx, dy);
		isSwapped = true;
	}
	int p = 2 * dy - dx;
	int twoDy = 2 * dy, twoDyMinusDx = 2 * (dy - dx);
	int x, y;

	if (x0 > xEnd) {
		x = xEnd;
		y = yEnd;
		xEnd = x0;
	}
	else {
		x = x0;
		y = y0;
	}
	if (isSwapped) {
		setPixel(PixelBuffer, y, x, width, height);
	}
	else {
		setPixel(PixelBuffer, x, y, width, height);
	}

	while (x < xEnd) {
		x++;
		if (p < 0)
			p += twoDy;
		else {
			(yEnd - y0 > 0) ? y++ : y--;
			p += twoDyMinusDx;
		}
		if (isSwapped) {
			setPixel(PixelBuffer, y, x, width, height);
		}
		else {
			setPixel(PixelBuffer, x, y, width, height);
		}
	}
	return true;
}

unsigned char encode(Point pt, Point winMin, Point winMax)
{
	GLubyte code = 0x00;
	if (pt.x < winMin.x)
		code = code | winLeftBitCode;
	if (pt.x > winMax.x)
		code = code | winRightBitCode;
	if (pt.y < winMin.y)
		code = code | winBottomBitCode;
	if (pt.y > winMax.y)
		code = code | winTopBitCode;
	return code;
}

void swapPts(Point * p1, Point * p2)
{
	Point tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}

void swapCodes(unsigned char *c1, unsigned char *c2)
{
	unsigned char tmp;
	tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}

bool lineClipCohStuth(Point winMin, Point winMax, Point &p1, Point &p2)
{
	unsigned char code1, code2;
	int done = false, plotLine = false;
	float m;

	while (!done)
	{
		code1 = encode(p1, winMin, winMax);
		code2 = encode(p2, winMin, winMax);
		if (accept(code1, code2))
		{
			done = true;
			plotLine = true;
		}
		else
			if (reject(code1, code2))
				done = true;
			else
			{
				if (inside(code1))
				{
					swapPts(&p1, &p2);
					swapCodes(&code1, &code2);
				}
				if (p2.x != p1.x)
					m = (p2.y - p1.y) / (p2.x - p1.x);
				if (code1 & winLeftBitCode)
				{
					p1.y += (winMin.x - p1.x) * m;
					p1.x = winMin.x;
				}
				else if (code1 & winRightBitCode)
				{
					p1.y += (winMax.x - p1.x) * m;
					p1.x = winMax.x;
				}
				else if (code1 & winBottomBitCode)
				{
					if(p2.x != p1.x)
						p1.x += (winMin.y - p1.y) * m;
					p1.y = winMin.y;
				}
				else if (code1 & winTopBitCode)
				{
					if (p2.x != p1.x)
						p1.x += (winMax.y - p1.y) * m;
					p1.y = winMax.y;
				}
			}
	}
	return plotLine;
}

int insideP(Point p, Boundary b, Point wMin, Point wMax)
{
	switch (b) {
	case Left:		if (p.x < wMin.x) return false; break;
	case Right:		if (p.x > wMax.x) return false; break;
	case Bottom:	if (p.y < wMin.y) return false; break;
	case Top:		if (p.y > wMax.y) return false; break;
	}
	return true;
}

int cross (Point p1, Point p2, Boundary winEdge, Point wMin, Point wMax)
{
	if (insideP(p1, winEdge, wMin, wMax) == insideP(p2, winEdge, wMin, wMax))
		return false;
	else
		return true;
}

Point intersect(Point p1, Point p2, Boundary winEdge, Point wMin, Point wMax)
{
	Point iPt;
	float m;
	
	if (p1.x != p2.x) m = (p1.y - p2.y) / (p1.x - p2.x);
	switch (winEdge) {
	case Left:
		iPt.x = wMin.x;
		iPt.y = p2.y + (wMin.x - p2.x) * m;
		break;
	case Right:
		iPt.x = wMax.x;
		iPt.y = p2.y + (wMax.x - p2.x) * m;
		break;
	case Bottom:
		iPt.y = wMin.y;
		if (p1.x != p2.x) iPt.x = p2.x + (wMin.y - p2.y) / m;
		else iPt.x = p2.x;
		break;
	case Top:
		iPt.y = wMax.y;
		if (p1.x != p2.x) iPt.x = p2.x + (wMax.y - p2.y) / m;
		else iPt.x = p2.x;
		break;
	}
	return iPt;
}

void clipPoint(Point p, Boundary winEdge, Point wMin, Point wMax, Point * pOut, int * cnt, Point*  first[], Point * s)
{
	Point iPt;
	if (!first[winEdge]) {
		Point * np = new Point;
		*np = p;
		first[winEdge] = np;
	}
	else
		if (cross(p, s[winEdge], winEdge, wMin, wMax)) {
			iPt = intersect( p, s[winEdge], winEdge, wMin, wMax);
			if (winEdge < Top)
				clipPoint(iPt, (Boundary)((int)winEdge + 1), wMin, wMax, pOut, cnt, first, s);
			else {
				pOut[*cnt] = iPt; (*cnt)++;
			}
		}

	s[winEdge] = p;
	if(insideP(p, winEdge, wMin, wMax))
		if(winEdge < Top)
			clipPoint(p, (Boundary)((int)winEdge + 1), wMin, wMax, pOut, cnt, first, s);
		else {
			pOut[*cnt] = p; (*cnt)++;
		}
}

void closeClip(Point wMin, Point wMax, Point *pOut, int *cnt, Point* first[], Point *s)
{
	Point pt;
	Boundary winEdge;

	for (winEdge = Left; winEdge <= Top; winEdge++) {
		if(cross(s[winEdge], *first[winEdge], winEdge, wMin, wMax)) {
			pt = intersect(s[winEdge],*first[winEdge],winEdge,wMin,wMax);
			if(winEdge < Top)
				clipPoint(pt, (Boundary)((int)winEdge + 1), wMin, wMax, pOut, cnt, first, s);
			else {
				pOut[*cnt] = pt; (*cnt)++;
			}
		}
	}
}

int polygonClipSuthHodg(Point wMin, Point wMax, int n, Point* pIn, Point *pOut)
{
	Point* first[nClip] = { 0,0,0,0 }, s[nClip];
	int k, cnt = 0;

	for (k = 0; k < n; k++)
		clipPoint(pIn[k], Left, wMin, wMax, pOut, &cnt, first, s);
	closeClip(wMin, wMax, pOut, &cnt, first, s);
	return cnt;
}

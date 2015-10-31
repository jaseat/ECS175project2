
#ifndef GRAPHICSPROC_H
#define GRAPHICSPROC_H
typedef struct {
	int x;
	int y;
}Point;
typedef enum { Left, Right, Bottom, Top, END_BOUNDARY } Boundary;
Boundary& operator++(Boundary& b, int);

void setPixel(float* PixelBuffer, int x, int y, int width, int height, float red, float blue, float green);
bool DDADraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer, float red, float green, float blue);
void setPixel(float* PixelBuffer, int x, int y, int width, int height);
bool getPixel(float* PixelBuffer, int x, int y, int width, int height);
bool DDADraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer);
bool BresenhamDraw(int x0, int y0, int xEnd, int yEnd, int width, int height, float* PixelBuffer);
unsigned char encode(Point pt, Point winMin, Point winMax);
bool lineClipCohStuth(Point winMin, Point winMax, Point &p1, Point &p2);
int inisdeP(Point p, Boundary b, Point wMin, Point wMax);
int cross(Point p1, Point p2, Boundary winEdge, Point wMin, Point wMax);
void closeClip(Point wMin, Point wMax, Point *pOut, int *cnt, Point* first[], Point *s);
int polygonClipSuthHodg(Point wMin, Point wMax, int n, Point* pIn, Point *pOut);
#endif

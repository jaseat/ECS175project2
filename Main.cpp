#include <GL/glut.h>
#include <iostream>
#include <string>
#include "GraphicsProc.h"
#include "Polygon.h"
#include "Shape.h"
#include "Line.h"
#include <stdlib.h>
#include <cstring>
#include <limits>
#include <fstream>
#include "3DWireFrame.h"
#include "3DTransformations.h"

using namespace std;

float *PixelBuffer, *Buffer1, *Buffer2, *Buffer3;
int bufferSize;
int subBufferSize;
int height;
int width;
int MainWindow, Win1, Win2, Win3;

void display();
void display1();
void display2();
void display3();
void keyboard(unsigned char key, int x, int y);
void update();
void normalizeWireFrames();
vector<myPolygon*> listOfPolygons;
vector<Shape*> listOfShapes;
vector<WireFrame*> listOfWireFrames;
Point winMin, winMax;
fstream file;
bool save();
bool load();
bool removeRotate;

int main(int argc, char* argv[])
{
	height = 600;
	width = 600;

	/*cout << "Please set the height of the window: ";
	cin >> height;
	while (cin.fail()) {
		cout << "Error: Not an integer." << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Please set the height of the window: ";
		cin >> height;
	}
	cout << "Please set the width of the window: ";
	cin >> width;
	while (cin.fail()) {
		cout << "Error: Not an integer." << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Please set the width of the window: ";
		cin >> width;
	}*/

	winMin.x = 1; winMin.y = 1;
	winMax.x = width-1; winMax.y = height-1;

	//Need to multiply height and width by three because each point is represented by a red green and blue value
	PixelBuffer = new float[height*width * 3];
	bufferSize = height*width * 3 * sizeof(float);
	Buffer1 = new float[(height / 2)*(width / 2) * 3];
	Buffer2 = new float[(height / 2)*(width / 2) * 3];
	Buffer3 = new float[(height / 2)*(width / 2) * 3];
	subBufferSize = (height / 2)*(width / 2) * 3 * sizeof(float);



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(width+1, height);
	glutInitWindowPosition(100, 100);

	MainWindow = glutCreateWindow("Project 2");
	glClearColor(0, 0, 0, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	Win1 = glutCreateSubWindow(MainWindow, (0), (0), (width / 2), (height / 2));
	glutDisplayFunc(display1);
	glutKeyboardFunc(keyboard);
	Win2 = glutCreateSubWindow(MainWindow, (0), (height / 2), (width / 2), (height / 2));
	glutDisplayFunc(display2);
	glutKeyboardFunc(keyboard);
	Win3 = glutCreateSubWindow(MainWindow, (width / 2), (0), (width / 2), (height / 2));
	glutDisplayFunc(display3);
	glutKeyboardFunc(keyboard);
	
	removeRotate = false;


	glutMainLoop();
	return 0;
}


void display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	memset(PixelBuffer, 0, bufferSize);

	//draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, PixelBuffer);

	//window refresh
	//glEnd();
	display1();
	display2();
	display3();
	glFlush();

}

void display1()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawPixels(width/2, height/2, GL_RGB, GL_FLOAT, Buffer1);
	glFlush();
}

void display2()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawPixels(width / 2, height / 2, GL_RGB, GL_FLOAT, Buffer2);
	glFlush();
}

void display3()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawPixels(width / 2, height / 2, GL_RGB, GL_FLOAT, Buffer3);
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	string command;
	if (removeRotate)
	{
		listOfWireFrames.pop_back();
		removeRotate = false;
		update();
	}
	if (key == 27/*ESC*/) {
		while (command != "continue") {
			cout << "Please enter a command: ";
			cin >> command;
			cout << "You entered: " << command << endl;
			
			if(command == "continue"){
				cout << "Continuing Program" << endl;
				break;
			}
			if (command == "dda") {
				int x0, y0, xEnd, yEnd;
				cout << "Please specify starting coordinates (formated as X Y no parenthesis): ";
				cin >> x0;
				cin >> y0;
				cout << "You specified " << x0 << " and " << y0 << endl;
				cout << "Please specify ending coordinates (formated as X Y no parenthesis): ";
				cin >> xEnd;
				cin >> yEnd;
				cout << "You specified " << xEnd << " and " << yEnd << endl;
				if (!DDADraw(x0, y0, xEnd, yEnd, width, height, PixelBuffer))
					cout << "Invalid Coordinates" << endl;
				else
				{
					Line *l = new Line(x0, y0, xEnd, yEnd, width, height, PixelBuffer, 0);
					listOfShapes.push_back(l);
				}
				break;
			}	
			if (command == "bresenham") {
				int x0, y0, xEnd, yEnd;
				cout << "Please specify starting coordinates (formated as X Y no parenthesis): ";
				cin >> x0;
				cin >> y0;
				cout << "You specified " << x0 << " and " << y0 << endl;
				cout << "Please specify ending coordinates (formated as X Y no parenthesis): ";
				cin >> xEnd;
				cin >> yEnd;
				cout << "You specified " << xEnd << " and " << yEnd << endl;
				if (!BresenhamDraw(x0, y0, xEnd, yEnd, width, height, PixelBuffer))
					cout << "Invalid Coordinates" << endl;
				else
				{
					Line *l = new Line(x0, y0, xEnd, yEnd, width, height, PixelBuffer, 1);
					listOfShapes.push_back(l);
				}
				break;
			}
			if (command == "polygon") {
				int numofpoints;
				Point p;
				vector<Point> listofPoints;
				cout << "Please specify number of points: ";
				cin >> numofpoints;
				for (int i = 0; i < numofpoints; i++) {
					cout << "Please specify coordinates of point " << i << " (formated as X Y no parenthesis): ";
					cin >> p.x;
					cin >> p.y;
					listofPoints.push_back(p);
				}
				myPolygon* poly = new myPolygon(listofPoints,PixelBuffer,width,height);
				poly->draw();
				listOfPolygons.push_back(poly);
				listOfShapes.push_back(poly);
				break;
			}
			if (command == "wireframe") {
				int numofpoints, numofedges;
				wcPt3d p;
				edge e;
				vector<wcPt3d> listofPoints;
				vector<edge> listofEdges;
				cout << "Please specify number of points: ";
				cin >> numofpoints;
				for (int i = 0; i < numofpoints; i++) {
					cout << "Please specify coordinates of point " << i << " (formated as X Y Z ): ";
					cin >> p.x;
					cin >> p.y;
					cin >> p.z;
					listofPoints.push_back(p);
				}
				cout << "Please specify number of edges: ";
				cin >> numofedges;
				for (int i = 0; i < numofedges; i++) {
					cout << "Please specify begin and end of edge " << i << " (formated as B E no parenthesis): ";
					cin >> e.begin;
					cin >> e.end;
					listofEdges.push_back(e);
				}
				WireFrame* wireframe = new WireFrame(listofPoints, listofEdges, Buffer1, Buffer2, Buffer3, width, height);
				listOfWireFrames.push_back(wireframe);
				normalizeWireFrames();
				wireframe->draw();
				
				break;
			}
			if (command == "translate")
			{
				int maxPoly = listOfWireFrames.size();
				int pId = 100;
				float tx, ty, tz;
				cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
				cin >> pId;
				while (pId >= maxPoly || pId < 0)
				{
					cout << "Incorrect ID" << endl;
					cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
					cin >> pId;
				}
				cout << "Please enter TX TY TZ (formated as TX TY TZ no parenthesis): ";
				cin >> tx;
				cin >> ty;
				cin >> tz;
				listOfWireFrames[pId]->translate(tx, ty, tz);
				update();
				break;
			}
			if (command == "rotate")
			{
				int maxPoly = listOfWireFrames.size();
				int pId = 100;
				double theta;
				char answ;
				wcPt3d p1, p2;
				cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
				cin >> pId;
				while (pId >= maxPoly || pId < 0)
				{
					cout << "Incorrect ID" << endl;
					cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
					cin >> pId;
				}
				cout << "Please enter theta: ";
				cin >> theta;
				cout << "Points should be in world coordinates" << endl;
				cout << "Please enter point 1 of axis ( X Y Z ): ";
				cin >> p1.x;
				cin >> p1.y;
				cin >> p1.z;
				cout << "Please enter point 2 of axis ( X Y Z ): ";
				cin >> p2.x;
				cin >> p2.y;
				cin >> p2.z;
				edge e;
				e.begin = 1;
				e.end = 2;
				vector<wcPt3d> lop;
				lop.push_back(p1); lop.push_back(p2);
				vector<edge> loe;
				loe.push_back(e);
				WireFrame* wf = new WireFrame(lop, loe, Buffer1, Buffer2, Buffer3, width, height, 1.0, 0.0, 0.0);
				listOfWireFrames.push_back(wf);
				removeRotate = true;
				listOfWireFrames[pId]->rotate(p1, p2, theta);
				update();
				break;
			}
			if (command == "scale")
			{
				int maxPoly = listOfWireFrames.size();
				int pId = 100;
				float sx, sy, sz;
				cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
				cin >> pId;
				while (pId >= maxPoly || pId < 0)
				{
					cout << "Incorrect ID" << endl;
					cout << "Please specify wireframe ID (range 0 - " << maxPoly - 1 << "): ";
					cin >> pId;
				}
				cout << "Please enter SX SY SZ (formated SX SY SZ no parenthesis): ";
				cin >> sx;
				cin >> sy;
				cin >> sz;
				listOfWireFrames[pId]->scale(sx, sy, sz);
				update();
				break;
			}
			if (command == "clip")
			{
				cout << "Please specify winMin (formated as X Y no parenthesis): ";
				cin >> winMin.x;
				cin >> winMin.y;
				cout << "Please specify winMax (formated as X Y no parenthesis): ";
				cin >> winMax.x;
				cin >> winMax.y;
				update();
				break;
			}
			if (command == "save")
			{
				if (save())
					cout << "Save successful" << endl;
				else
					cout << "Save failed" << endl;
				break;
			}
			if (command == "load")
			{
				if (load())
					cout << "Load successful" << endl;
				else
					cout << "Load failed" << endl;
				break;
			}
			else {
				cout << "Invalid Command: " << command << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
	}

	display();
	glutSetWindow(Win1);
	glutPostRedisplay();
	glutSetWindow(Win2);
	glutPostRedisplay();
	glutSetWindow(Win3);
	glutPostRedisplay();
}

void update()
{
	memset(PixelBuffer, 0, bufferSize);
	memset(Buffer1, 0, subBufferSize);
	memset(Buffer2, 0, subBufferSize);
	memset(Buffer3, 0, subBufferSize);
	for (int k = 0; k < listOfWireFrames.size(); k++)
	{
		normalizeWireFrames();
		listOfWireFrames[k]->draw();
	}
	glutSetWindow(Win1);
	glutPostRedisplay();
	glutSetWindow(Win2);
	glutPostRedisplay();
	glutSetWindow(Win3);
	glutPostRedisplay();
}

bool save()
{
	if (listOfWireFrames.empty())
		return false;
	file.open("wireframe.txt", ios::out | ios::trunc);
	int size = listOfWireFrames.size();
	file << size << endl;
	for (int i = 0; i < size; i++)
	{
		vector<wcPt3d> ListPoints = listOfWireFrames[i]->getPoints();
		int numPoints = ListPoints.size();
		vector<edge> ListEdges = listOfWireFrames[i]->getEdges();
		int numEdges = ListEdges.size();
		file << numPoints << endl;
		for (int j = 0; j < numPoints; j++)
		{
			file << ListPoints[j].x << " " << ListPoints[j].y << " " << ListPoints[j].z << endl;
		}
		file << numEdges << endl;
		for (int j = 0; j < numEdges; j++)
		{
			file << ListEdges[j].begin << " " << ListEdges[j].end <<  endl;
		}
	}
	file.close();
	return true;
}

bool load()
{
	file.open("wireframe.txt", ios::in);
	if (file.peek() == ifstream::traits_type::eof()) {
		file.close();
		return false;
	}
	listOfShapes.clear();
	listOfPolygons.clear();
	listOfWireFrames.clear();
	int size;
	file >> size;
	for (int i = 0; i < size; i++)
	{
		int numPoints, numEdge;
		file >> numPoints;
		vector<wcPt3d> listofPoints;
		vector<edge> listofEdges;
		for (int j = 0; j < numPoints; j++)
		{
			wcPt3d p;
			
			file >> p.x;
			file >> p.y;
			file >> p.z;
			listofPoints.push_back(p);
		}
		file >> numEdge;
		for (int j = 0; j < numEdge; j++)
		{
			edge e;

			file >> e.begin;
			file >> e.end;
			listofEdges.push_back(e);
		}
		WireFrame* wf = new WireFrame(listofPoints, listofEdges, Buffer1, Buffer2, Buffer3, width, height);
		listOfWireFrames.push_back(wf);
	}
	update();
	file.close();
	return true;

}

void normalizeWireFrames()
{
	int size = listOfWireFrames.size();
	float dx, dy, dz, MasterMin, MasterMax;
	wcPt3d minPoint, maxPoint;
	WireFrame* wf = listOfWireFrames[0];
	minPoint = wf->getPoint(0); maxPoint = wf->getPoint(0);

	for (int i = 0; i < size; i++)
	{
		listOfWireFrames[i]->getMinMax(&minPoint, &maxPoint);
	}
	/*dx = maxPoint.x - minPoint.x; dy = maxPoint.y - minPoint.y; dz = maxPoint.z - minPoint.z;
	MasterMin = minPoint.x; MasterMax = maxPoint.x;
	if (dy > MasterMax - MasterMin)
	{
		MasterMin = minPoint.y; MasterMax = maxPoint.y;
	}
	if (dz > MasterMax - MasterMin)
	{
		MasterMin = minPoint.z; MasterMax = maxPoint.z;
	}*/
	MasterMin = minPoint.x;
	if (minPoint.y < MasterMin)
		MasterMin = minPoint.y;
	if (minPoint.z < MasterMin)
		MasterMin = minPoint.z;
	MasterMax = maxPoint.x;
	if (maxPoint.y > MasterMax)
		MasterMax = maxPoint.y;
	if (maxPoint.z > MasterMax)
		MasterMax = maxPoint.z;

	for (int i = 0; i < size; i++)
	{
		listOfWireFrames[i]->normalizePoints(MasterMin-5, MasterMax+5);
	}
}
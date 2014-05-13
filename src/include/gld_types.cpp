#include "gld_types.h"
#include <math.h>

inputManager input;
Box canvas, form;


void Point::set(int sx, int sy) {
	x = sx; y = sy;
};
Point point(int x, int y) {
	Point result;
	result.set(x, y);
	return result;
};

double distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
};
double pointDistance(Point p1, Point p2) {
	return sqrt(pow((long double)(p1.x - p2.x), 2) + pow((long double)(p1.y - p2.y), 2));
};

Point Box::getCenterPosition() {
	Point result;
	result.set((left + right) / 2, (top + bottom) / 2);
	return result;
};
void Box::set(int x1, int y1, int x2, int y2) 
{
	if (x1 > x2)
		std::swap(x1, x2);
	if (y1 > y2)
		std::swap(y1, y2);
	top = y1; left = x1; bottom = y2; right = x2;
};
void Box::set(int x, int y, int size)
{
	left = x;
	top = y; 
	right = x+size;
	bottom = y+size;
};
void Box::set(Box border) {
	set(border.left, border.top, border.right, border.bottom);
};
bool Box::contains(int x, int y) 
{
	return (x > left && x <= right && y > top && y <= bottom);
};
bool Box::contains(Point position)
{
	return (position.x >= left && position.x <= right && position.y >= top && position.y <= bottom);
};
void Box::move(int vx, int vy) 
{
	top += vy; bottom += vy; left += vx; right += vx;
};
void Box::moveTo(int x, int y) 
{
	int width = right - left;
	int height = bottom - top;
	top = y; bottom = y + height;
	left = x; right = x + width;
};
int Box::getWidth() 
{
	return right - left;
};
int Box::getHeight() 
{
	return bottom - top;
};
void Box::setWidth(int newWidth)
{
	if (newWidth < 0) 
		return;
	right = left + newWidth;
};
void Box::setHeight(int newHeight)
{
	if (newHeight < 0)
		return;
	bottom = top + newHeight;
};
void Box::setSize(int newWidth, int newHeight)
{
	setWidth(newWidth);
	setHeight(newHeight);
};

bool fileExists(char* fileName) {
	std::ifstream infile(fileName);
	bool result = infile.good();
	infile.close();
	return result;
};
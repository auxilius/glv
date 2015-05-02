#include <iomanip>
#include "gld_types.h"


inputManager input;
Box canvas;
Box form;
HWND windowHandle;
HDC deviceContext;
std::string SAVE_PATH = "";

GLcolor::GLcolor() {
	set(0.0f, 0.0f, 0.0f);
};
GLcolor::GLcolor(GLfloat r, GLfloat g, GLfloat b) {
	set(r, g, b);
};
void GLcolor::set(GLfloat r, GLfloat g, GLfloat b) {
	R = r;
	G = g;
	B = b;
};

Point::Point() {
	x = y = 0;
};
Point::Point(int cx, int cy) {
	x = cx;
	y = cy;
};

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

Point Box::getPosition() {
	Point result;
	result.set(left, top);
	return result;
};
Point Box::getCenterPosition() {
	Point result;
	result.set((left + right) / 2, (top + bottom) / 2);
	return result;
};
Box::Box() {
	set(0,0,0,0);
};
Box::Box(int x1, int y1, int x2, int y2) {
	set(x1,y1,x2,y2);
};
Box::Box(int x, int y, int size) {
	set(x,y,size);
};
Box::Box(const Box &border) {
	set(border);
};
void Box::set(const Box &border) {
	set(border.left, border.top, border.right, border.bottom);
};
void Box::set(int x1, int y1, int x2, int y2) {
	if (x1 > x2)
		std::swap(x1, x2);
	if (y1 > y2)
		std::swap(y1, y2);
	top = y1; left = x1; bottom = y2; right = x2;
};
void Box::set(int x, int y, int size) {
	left = x;
	top = y; 
	right = x+size;
	bottom = y+size;
};
bool Box::contains(int x, int y) 
{
	return (x > left && x < right && y > top && y < bottom);
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



#pragma region Utils

bool fileExists(const char* fileName) {
	std::ifstream infile(fileName);
	bool result = infile.good();
	infile.close();
	return result;
};

bool dirExists(const std::string& dirName_in) {
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; 
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true; 
	return false;
};

bool fileCreate(const char* fileName) {
	std::ofstream outfile ( fileName );
	bool result = outfile.good();
	outfile.close();
	return result;
};

std::string pathToFile(const char* fname) {
	return SAVE_PATH + fname;
};

char * stringToChar(std::string str) {
	char * writable = new char[str.size() + 1];
	strcpy_s(writable, str.size() + 1, str.c_str());
	return writable;
};

std::string trim(std::string str) {
	if (str.length() == 0)
		return str;
	int s = 0;
	int e = str.length()-1;
	while ( (str[s] == ' ') && (s < e) ) 
		s++;
	while ( (str[e] == ' ') && (s < e) ) 
		e--;
	return str.substr(s, e-s+1);
};

#pragma endregion
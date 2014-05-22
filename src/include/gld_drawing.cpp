#include "gld_drawing.h"
#include <iostream>
#include <gl/glew.h>
#include <gl/glf.h>


double actualFontSize = 12;

void reloadProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, canvas.width, canvas.height);
	glOrtho(-1, canvas.width - 1, canvas.height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
};

void initFonts() {
	glfInit();
	char * fontFile = "font_arial.glf";
	if (!fileExists(fontFile)) {
		MessageBox(0, L"Error loading font", L"Font cannot be loaded, font file is probably missing", MB_OK);
		return;
	}
	int font = glfLoadFont(fontFile);
	glfSetCurrentFont(font);
	glfSetAnchorPoint(GLF_LEFT_UP);
};

unsigned getTextWidth(char * text) {
	float xmin, ymin, xmax, ymax;
	glfGetStringBounds(text, &xmin, &ymin, &xmax, &ymax);
	return (unsigned)(xmax - xmin)*actualFontSize+actualFontSize;
};
unsigned getTextHeight(char * text) {
	float xmin, ymin, xmax, ymax;
	glfGetStringBounds(text, &xmin, &ymin, &xmax, &ymax);
	return (unsigned)(ymax - ymin)*actualFontSize+(float)actualFontSize/2;
};
void setFontSize(double size) {
	actualFontSize = size;
}
void textPrint(int X, int Y, char * text) {
	glPushMatrix();
	glLoadIdentity();
	glTranslatef((GLfloat)X, (GLfloat)Y, 0.0f);
	glScalef((GLfloat)actualFontSize, -(GLfloat)actualFontSize, 0.0f);
	glfDrawSolidString(text);
	glPopMatrix();
};
void textPrintCentered(int X, int Y, char* text) {
	textPrint(X - getTextWidth(text) / 2, Y - getTextHeight(text) / 2, text);
};
void textPrint(int X, int Y, std::string string) {
	char *cstr = new char[string.length() + 1];
	strcpy(cstr, string.c_str());
	textPrint(X,Y, cstr);
	delete[] cstr;
};
void textPrintCentered(int X, int Y, std::string string) {
	char *cstr = new char[string.length() + 1];
	strcpy(cstr, string.c_str());
	textPrintCentered(X, Y, cstr);
	delete[] cstr;
};


void glVerticeSquare(Box border){
	glVerticeSquare(border.left, border.top, border.right, border.bottom);
};
void glVerticeSquare(int x, int y, int size) {
	glVerticeSquare(x, y, x+size, y+size);
};
void glVerticeSquare(int x1, int y1, int x2, int y2) {
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
};

void drawRect(GLenum mode, Box border) {
	glBegin(mode);
	glVerticeSquare(border);
	glEnd();
};
void drawLine(int x1, int y1, int x2, int y2) {
	glBegin(GL_LINES);
	glVertex2f((GLfloat)x1, (GLfloat) y1);
	glVertex2f((GLfloat)x2, (GLfloat) y2);
	glEnd();
};

void drawSwitchIcon(int x, int y, int size) {
	static GLfloat vertices[] = {
		0.000000, 0.000000,
		-0.54054, -0.24475,
		-0.43845, -0.39981,
		-0.28763, -0.33971,
		-0.00796, -0.44505,
		0.065734, -0.58972,
		0.244751, -0.54054,
		0.234205, -0.37853,
		0.420811, -0.14510,
		0.581173, -0.11971,
		0.589723, 0.065734,
		0.432378, 0.105770,
		0.268042, 0.355375,
		0.293451, 0.515734,
		0.119718, 0.581173,
		0.033019, 0.443901,
		-0.25515, 0.364740,
		-0.39981, 0.438459,
		-0.51573, 0.293451,
		-0.41197, 0.168576,
		-0.42573, -0.12995,
		-0.19015, -0.05325,
		-0.12253, -0.15485,
		-0.00811, -0.19730,
		0.109406, -0.16439,
		0.185140, -0.06869,
		0.190156, 0.053251,
		0.122539, 0.154852,
		0.008117, 0.197305,
		-0.10940, 0.164394,
		-0.18514, 0.068690
	};
	static GLbyte indices[] = { 
		2, 3, 20, 1,
		17, 18, 19, 16,
		13, 14, 15, 12,
		9, 10, 11, 8,
		5, 6, 7, 4,
		16, 19, 30, 29,
		15, 16, 29, 28,
		12, 15, 28, 27,
		11, 12, 27, 26,
		8, 11, 26, 25,
		7, 8, 25, 24,
		4, 7, 24, 23,
		3, 4, 23, 22,
		20, 3, 22, 21,
		19, 20, 21, 30
	};
	glPushMatrix();
	glLoadIdentity();
	glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);
	glScalef((GLfloat)size, (GLfloat)size, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawElements(GL_QUADS, 60, GL_UNSIGNED_BYTE, indices);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
<<<<<<< HEAD
};

void glColor(unsigned char r, unsigned char g, unsigned char b) {
	glColor3f((GLfloat)r / 255, (GLfloat)g / 255, (GLfloat)b / 255);
};
=======
};
>>>>>>> d897c36f0ef0ba704fa81affdc366c35607ab3d0

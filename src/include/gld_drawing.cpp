#include "gld_drawing.h"
#include "gld_constants.h"

#include <iostream>
#include <gl/glew.h>

#include "glv_Shaders.h"


Color clWhite	= {1.00f, 1.00f, 1.00f, 1.0f};
Color clGray90	= {0.90f, 0.90f, 0.90f, 1.0f};
Color clGray80	= {0.80f, 0.80f, 0.80f, 1.0f};
Color clGray70  = {0.70f, 0.70f, 0.70f, 1.0f};
Color clGray60	= {0.60f, 0.60f, 0.60f, 1.0f};
Color clGray50  = {0.50f, 0.50f, 0.50f, 1.0f};
Color clGray40	= {0.40f, 0.40f, 0.40f, 1.0f};
Color clGray30	= {0.30f, 0.30f, 0.30f, 1.0f};
Color clGray20	= {0.20f, 0.20f, 0.20f, 1.0f};
Color clGray10  = {0.10f, 0.10f, 0.10f, 1.0f};
Color clBlack	= {0.00f, 0.00f, 0.00f, 1.0f};

Color clConfBack	= {0.050f, 0.100f, 0.020f, 1.0f};
Color clConfGrid	= {0.075f, 0.100f, 0.020f, 1.0f};
Color clConfBoxMes	= {0.240f, 0.270f, 0.210f, 1.0f};
Color clConfBoxVal	= {0.430f, 0.430f, 0.290f, 1.0f};
Color clConfBoxTex	= {0.520f, 0.570f, 0.430f, 1.0f};

Color clBtnBack			= {0.75f, 0.75f, 0.75f, 1.0f};
Color clBtnBackHover	= {0.85f, 0.85f, 0.85f, 1.0f};
Color clBtnBackChecked	= {0.85f, 0.82f, 0.50f, 1.0f};
Color clBtnBorder		= {0.50f, 0.50f, 0.50f, 1.0f};
Color clBtnBorderHover	= {0.65f, 0.65f, 0.65f, 1.0f};
Color clBtnText			= {0.10f, 0.10f, 0.10f, 1.0f};
Color clBtnTextHover	= {0.20f, 0.20f, 0.20f, 1.0f};


double actualFontSize = 12;

void reloadProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, canvas.width, canvas.height);
	glOrtho(-1, canvas.width - 1, canvas.height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
};

Font *fontMain, *fontSmall, *fontLarge, *fontHuge;

void initFonts() {
	fontMain = new Font(deviceContext, ffVerdana, 15);
	fontSmall = new Font(deviceContext, ffVerdana, 10);
	fontLarge = new Font(deviceContext, ffVerdana, 20);
	fontHuge = new Font(deviceContext, ffVerdana, 30);
};

void glVerticeSquare(Box *border){
	glVerticeSquare(border->left, border->top, border->right, border->bottom);
};

void glVerticeSquare(int x, int y, int size) {
	glVerticeSquare(x, y, x+size, y+size);
};

void glVerticeSquare(int x1, int y1, int x2, int y2) {
	glVertex2f((GLfloat)x1, (GLfloat)y1);
	glVertex2f((GLfloat)x2, (GLfloat)y1);
	glVertex2f((GLfloat)x2, (GLfloat)y2);
	glVertex2f((GLfloat)x1, (GLfloat)y2);
};

void drawRect(GLenum mode, Box *border) {
	//glBegin(mode);
	//glVerticeSquare(border);
	//glEnd();
	shader->bind(progRenderBox);
	GLfloat colors[] = {1.0, 0.5, 1.0, 1.0};
	shader->setUniform4f("color", colors);
	GLfloat corners[] = {(GLfloat)border->left, (GLfloat)border->top, (GLfloat)border->right, (GLfloat)border->bottom};
	shader->setUniform4f("corners", corners);
	shader->setUniform1i("width", canvas.width);
	shader->setUniform1i("height", canvas.height);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	shader->unbind();
};

void drawLine(int x1, int y1, int x2, int y2) {
	glBegin(GL_LINES);
	glVertex2f((GLfloat)x1, (GLfloat) y1);
	glVertex2f((GLfloat)x2, (GLfloat) y2);
	glEnd();
};


void glColor(unsigned char r, unsigned char g, unsigned char b) {
	glColor3f((GLfloat)r / 255, (GLfloat)g / 255, (GLfloat)b / 255);
};



GLcolor valToColor(float value, unsigned colormap) {
	if (colormap == COLOR_MAP_BLUERED)
		return valToColor_BlueRed(value);
	else if (colormap == COLOR_MAP_RAINBOW)
		return valToColor_Rainbow(value);
	else {
		return GLcolor(1.0f, 0.0f, 1.0f);
	}
};

GLcolor valToColor_Rainbow(float value) {
	GLcolor result;
	if (value >= 0.0f && value < 0.2f) {
		value = value / 0.2f;
		result.set(0.0f, value, 1.0f);
	}
	else if (value >= 0.2f && value < 0.4f) {
		value = (value - 0.2f) / 0.2f;
		result.set(0.0f, 1.0f, 1.0f - value);
	}
	else if (value >= 0.4f && value < 0.6f) {
		value = (value - 0.4f) / 0.2f;
		result.set(value, 1.0f, 0.0f);
	}
	else if (value >= 0.6f && value < 0.8f) {
		value = (value - 0.6f) / 0.2f;
		result.set(1.0f, 1.0f - value, 0.0f);
	}
	else if (value >= 0.8f && value <= 1.0f) {
		value = (value - 0.8f) / 0.2f;
		result.set(1.0f, 0.0f, value);
	}
	else if (value > 1.0f)
		result.set(1.0f, 1.0f, 1.0f);
	return result;
};

GLcolor valToColor_BlueRed(float value) {
	GLcolor result( 1.0f-value, 0.0f, value);
	return result;
};

void drawBox(Box *border, Color color, bool filled /*= true*/) {
	shader->bind(progRenderBox);
	shader->setUniform1i("width", canvas.width);
	shader->setUniform1i("height", canvas.height);
	shader->setUniform4f("color", color);
	GLfloat corners[] = {(GLfloat)border->left, (GLfloat)border->top, (GLfloat)border->right, (GLfloat)border->bottom};
	shader->setUniform4f("corners", corners);
	glDrawArrays( filled?GL_TRIANGLE_FAN:GL_LINE_LOOP, 0, 4);
	shader->unbind();
};

void drawPoint(Point *position, Color color, int size /*= 1*/) {
	glEnable(GL_POINT_SMOOTH);
	glPointSize((GLfloat)size);
	shader->bind(progRenderBox);
	shader->setUniform1i("width", canvas.width);
	shader->setUniform1i("height", canvas.height);
	shader->setUniform4f("color", color);
	GLfloat corners[] = {(GLfloat)position->x, (GLfloat)position->y, 0.0f, 0.0f};
	shader->setUniform4f("corners", corners);
	glDrawArrays( GL_POINTS, 0, 1);
	shader->unbind();
};

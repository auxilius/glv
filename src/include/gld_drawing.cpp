#include "gld_drawing.h"
#include "gld_constants.h"

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

Font *fontMain, *fontSmall, *fontLarge, *fontHuge;

void initFonts() {
	fontMain = new Font(deviceContext, ffVerdana, 15);
	fontSmall = new Font(deviceContext, ffVerdana, 10);
	fontLarge = new Font(deviceContext, ffVerdana, 20);
	fontHuge = new Font(deviceContext, ffVerdana, 30);
};

void glVerticeSquare(Box border){
	glVerticeSquare(border.left, border.top, border.right, border.bottom);
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
	/*static GLfloat vertices[]; = {
		0.000000.0f, 0.000000.0f,
		-0.54054.0f, -0.24475.0f,
		-0.43845.0f, -0.39981.0f,
		-0.28763.0f, -0.33971.0f,
		-0.00796.0f, -0.44505.0f,
		0.065734.0f, -0.58972.0f,
		0.244751.0f, -0.54054.0f,
		0.234205.0f, -0.37853.0f,
		0.420811.0f, -0.14510.0f,
		0.581173.0f, -0.11971.0f,
		0.589723.0f, 0.065734.0f,
		0.432378.0f, 0.105770.0f,
		0.268042.0f, 0.355375.0f,
		0.293451.0f, 0.515734.0f,
		0.119718.0f, 0.581173.0f,
		0.033019.0f, 0.443901.0f,
		-0.25515.0f, 0.364740.0f,
		-0.39981.0f, 0.438459.0f,
		-0.51573.0f, 0.293451.0f,
		-0.41197.0f, 0.168576.0f,
		-0.42573.0f, -0.12995.0f,
		-0.19015.0f, -0.05325.0f,
		-0.12253.0f, -0.15485.0f,
		-0.00811.0f, -0.19730.0f,
		0.109406.0f, -0.16439.0f,
		0.185140.0f, -0.06869.0f,
		0.190156.0f, 0.053251.0f,
		0.122539.0f, 0.154852.0f,
		0.008117.0f, 0.197305.0f,
		-0.10940.0f, 0.164394.0f,
		-0.18514.0f, 0.068690.0f
	};
	static GLubyte indices[] = { 
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
	

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	
	glEnableClientState(GL_INDEX_ARRAY);
	glIndexPointer(GL_UNSIGNED_BYTE, 0, indices);

	glDrawElements(GL_QUADS, 60, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glPopMatrix();*/
};

void glColor(unsigned char r, unsigned char g, unsigned char b) {
	glColor3f((GLfloat)r / 255, (GLfloat)g / 255, (GLfloat)b / 255);
};

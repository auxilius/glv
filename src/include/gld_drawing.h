#ifndef H_OGL_DRAWING
#define H_OGL_DRAWING

#include <string>
#include <gl/glew.h>
#include "gld_types.h"

void reloadProjection();

////   TEXT DRAWING
void initFonts();

unsigned getTextWidth(char * text);
unsigned getTextHeight(char * text);

void setFontSize(double size);

void textPrint(int X, int Y, char * string);
void textPrintCentered(int X, int Y, char * string);
void textPrint(int X, int Y, std::string string);
void textPrintCentered(int X, int Y, std::string string);


////   GEOMETRY DRAWING
void glVerticeSquare(Box border);
void glVerticeSquare(int x, int y, int size);
void glVerticeSquare(int x1, int y1, int x2, int y2);
void drawRect(GLenum mode, Box border);
void drawLine(int x1, int y1, int x2, int y2);

void drawSwitchIcon(int x, int y, int size = 10);

#endif
#ifndef H_OGL_DRAWING
#define H_OGL_DRAWING

#include <string>
#include <gl/glew.h>
#include "gld_types.h"
#include "glfont.h"
#include "glv_Shaders.h"

////   COLORS
typedef GLfloat Color [4];
extern Color clWhite, clGray10, clGray20, clGray30, clGray40;
extern Color clGray50, clGray60, clGray70, clGray80, clGray90, clBlack;
extern Color clRed, clGreen, clBlue, clCyan, clMagenta, clYellow;

extern Color clConfBack, clConfGrid, clConfBoxMes, clConfBoxVal, clConfBoxTex;

extern Color clBtnBack, clBtnBorder, clBtnText;
extern Color clBtnBackHover, clBtnBorderHover, clBtnTextHover;
extern Color clBtnBackChecked;


////   OPEN_GL
void reloadProjection();

////   TEXT DRAWING
extern Font *fontMain, *fontSmall, *fontLarge, *fontHuge;

void initFonts();

////   GEOMETRY DRAWING
void glVerticeSquare(Box *border);
void glVerticeSquare(int x, int y, int size);
void glVerticeSquare(int x1, int y1, int x2, int y2);
void drawRect(GLenum mode, Box *border);
void drawLine(int x1, int y1, int x2, int y2);


void drawSwitchIcon(int x, int y, int size = 10);

void glColor(unsigned char r, unsigned char g, unsigned char b);

GLcolor valToColor(float value, unsigned colormap);
GLcolor valToColor_Rainbow(float value);
GLcolor valToColor_BlueRed(float value);

void drawBox(Box *border, Color color, bool filled = true);
void drawPoint(Point *position, Color color, int size = 1);

#endif
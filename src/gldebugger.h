#ifndef __H_GLDEBUGGER
#define __H_GLDEBUGGER

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

int gldInit(HGLRC glrcToShare = NULL, HDC dcToShare = NULL);

void gldStart();

void gldStop();

bool gldAddTexture(char * caption, GLuint texture);
bool gldAddLine(char * format, void * data);
bool gldAddLine(char * format, void * data[]);
bool gldAddModel(char * caption, GLuint vbo, unsigned count);
bool gldAddModelData(char * caption, float * data, float min = 0.0f, float max = 1.0f);
bool gldAddModelData(GLuint vboid, float * data, float min = 0.0f, float max = 1.0f);


#endif
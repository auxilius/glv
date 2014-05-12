#ifndef __H_GLDEBUGGER
#define __H_GLDEBUGGER

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

/* inicializuje kniznicu - ak nie su zadane parametre, automaticky sa nacitaju kontexty ktore su akurat nastavene */
int gldInit(HGLRC glrcToShare = NULL, HDC dcToShare = NULL);

/* spusti debuggovacie okno */
void gldStart();

/* vypne debugovacie okno */
void gldStop();


/* prida texturu - caption je ucivatelsky popis textury a druhy parameter je jej id */
bool gldAddTexture(char * caption, GLuint texture);

/* prida riadok do vypisu - format udava co sa ma vypisovat a data je smernik na data alebo pole smernikov, ktore sa budu vypisovat. Dostupne formaty dat su bool(%b), int(%i), unsigned(%u), float(%f), double(%d), char(%c) */
bool gldAddLine(char * format, void * data);
bool gldAddLine(char * format, void * data[]);

/* prida model do debuggera */
bool gldAddModel(char * caption, GLuint vbo, unsigned count);

/* nastavy data na model - da sa identifikovat model prvim parametrom, but jeho popisom caption, alebo jeho idckom */
bool gldAddModelData(char * caption, float * data, float min = 0.0f, float max = 1.0f);
bool gldAddModelData(GLuint vboid, float * data, float min = 0.0f, float max = 1.0f);

#endif
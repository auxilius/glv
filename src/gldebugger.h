#ifndef __H_GLDEBUGGER
#define __H_GLDEBUGGER

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

/* Inicializuje kniznicu - ak nie su zadane parametre, automaticky sa nacitaju kontexty ktore su aktualne nastavene */
int gldInit(HGLRC glrcToShare = NULL, HDC dcToShare = NULL);

/* Spusti debuggovacie okno */
void gldStart();

/* Vypne debugovacie okno */
void gldStop();

/* Prida texturu - caption je uzivatelsky popis textury a druhy parameter je jej id */
bool gldAddTexture(std::string caption, GLuint texture);

/* Prida riadok do vypisu - format udava co sa ma vypisovat a data je smernik na premennu, ktora sa bude vypisovat. 
Dostupne formatovanie je: bool(%b), int(%i), unsigned(%u), float(%f), double(%d), char(%c).
Nie je potrebne aby format korespondoval s vlozenymi udajmi, napr. smernik na bool mozeme formatovat ako %i */
bool gldAddLine(std::string format, void * data);
bool gldAddLine(std::string format, void * data[]);
bool gldAddLine(std::string caption, std::string format, void * data);
bool gldAddLine(std::string caption, std::string format, void * data[]);

/* Prida model a nazve ho podla parametru caption. Posledny parameter je pocet vertexov modelu */
bool gldAddModel(std::string caption, GLuint vbo, unsigned count);

/* Nastavy data na model - identifikacia modelu podla jeho nazvu alebo id */
bool gldAddModelData(std::string caption, float * data, float min = 0.0f, float max = 1.0f);
bool gldAddModelData(GLuint vboid, float * data, float min = 0.0f, float max = 1.0f);


#endif
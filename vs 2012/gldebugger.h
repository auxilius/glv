#ifndef __H_GLDEBUGGER
#define __H_GLDEBUGGER

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <string>
using namespace std;

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

//void gldAddArray(std::string caption, void * data, std::string format, int length, int lineLength = 0);
//void gldAddArray(std::string caption, int * data, std::string format, int length, int lineLength = 0);
//void gldAddArray(std::string caption, float * data, std::string format, int length, int lineLength = 0);

/* Prida model a nazve ho podla parametru caption. Posledny parameter je pocet vertexov modelu */
bool gldAddModel(std::string caption, unsigned count, GLuint vertices, GLenum type = GL_FLOAT);

/* Pridadi data modelu, ktory ma popis caption */
bool gldAddModelData(std::string caption, float * data, float min = 0.0f, float max = 1.0f);

/* Pridadi pole indexov modelu, ktory ma popis caption */
bool gldAddModelEdges(std::string caption, GLenum mode, unsigned count, GLuint indices, GLenum type = GL_UNSIGNED_INT);

/* Pridadi texturu a koordinaty modelu, ktory ma popis caption */
bool gldAddModelTexture(std::string caption, GLuint texture, GLuint coordinates, GLenum type = GL_FLOAT);

#endif
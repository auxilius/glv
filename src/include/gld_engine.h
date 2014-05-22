#ifndef H_GLDENGINE
#define H_GLDENGINE

#include "gld_types.h"
#include "gld_config.h"
#include <iostream>
#include <stdarg.h>
#include "gld_renderer.h"

enum RoomType { rmVisual, rmConfig };

class gldEngine {
private:
	bool needToRender;
	RoomType room;
	Box switchButton;
	gldConfigurator configurator;
	gldRenderer visualizer;
	void renderMenu();
	void processMenuClick(int x, int y, mouseButton button);
public:	
	void init();
	void render();
	void saveConfiguration();
	void onCanvasSizeChange(int width, int height);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);
	void onMouseMove(int x, int y);
	void onMouseWheel(signed short direction);

<<<<<<< HEAD
	void addTexture(const char * caption, const GLuint texture);
	void addValues(const char * caption, const char * format, void * data[]);
	void addModel(const char * caption, const GLuint vbo, const unsigned count);
	void addModelData(const char * caption, float * data, float min, float max);
=======
	void addTexture(char * caption, const GLuint texture);
	void addValue(char * caption, void * data);
	void addValues(char * caption, void * data[]);
	void addModel(char * caption, const GLuint vbo, const unsigned count);
	void addModelData(char * caption, float * data, float min, float max);
>>>>>>> d897c36f0ef0ba704fa81affdc366c35607ab3d0
	void addModelData(const GLuint vbo, float * data, float min, float max);
};

#endif
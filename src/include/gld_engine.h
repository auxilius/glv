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
	gldConfigurator configManager;
	gldRenderer visualizer;
	void renderMenu();
	void processMenuClick(int x, int y, mouseButton button);
public:	
	void init();
	void render();

	void saveConfig();
	void loadConfig();

	void onCanvasSizeChange(int width, int height);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);
	void onMouseMove(int x, int y);
	void onMouseWheel(signed short direction);

	void addTexture(const char * caption, const GLuint texture);
	void addValues(const char * caption, const char * format, void * data[]);
	void addModel(const char * caption, const unsigned count, const GLuint vertices, const GLuint indices);
	void addModelData(const char * caption, float * data, float min, float max);
	void addModelData(const GLuint vbo, float * data, float min, float max);
};

#endif
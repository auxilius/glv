#ifndef H_GLDENGINE
#define H_GLDENGINE

#include "gld_types.h"
#include "gld_config.h"
#include <iostream>
#include <stdarg.h>
#include "gld_visualizer.h"

enum RoomType { rmVisual, rmConfig };

class gldEngine {
private:
	bool needToRender;
	RoomType room;
	Box switchButton;
	
	void renderMenu();
	void processMenuClick(int x, int y, mouseButton button);
public:	
	gldRenderer visualizer;
	gldConfigurator configManager;

	void init();
	void render();

	void onCanvasSizeChange(int width, int height);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);
	void onMouseMove(int x, int y);
	void onMouseWheel(signed short direction);

};

#endif
#ifndef H_GLDENGINE
#define H_GLDENGINE

#include "gld_types.h"
#include "gld_config.h"
#include <iostream>
#include <stdarg.h>
#include <vector>
#include "gld_visualizer.h"

enum RoomType { rmVisual, rmConfig };

class gldEngine {
private:
	bool needToRender;
	RoomType room;
	Button button_modeSwitch; // for switching between visualization and customization
	PopupMenu menu_profileSelect;
	Button button_profileSelect;
	
	void panelCalculate();
	void panelRender();
	void panelMouseDown();
	void searchForConfigFiles();
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
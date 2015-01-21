#ifndef H_GLDENGINE
#define H_GLDENGINE

#include <iostream>
#include <stdarg.h>
#include <vector>

#include "gld_types.h"
#include "gld_config.h"
#include "gld_visualizer.h"

enum RoomType { rmVisual, rmConfig };

class gldEngine {
private:
	bool needToRender;
	RoomType room;
	Button button_modeSwitch;
	PopupMenu menu_profileSelect;
	PopupMenu menu_profileEdit;
	Button button_profileSelect;

	void menu_profileSelectLoad();
	
	void panelCalculate();
	void panelRender();
	bool panelMouseDown(mouseButton button);	
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
	void onKeyDown(char key);
};

#endif
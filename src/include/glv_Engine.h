#pragma once
#ifndef GLV_H_ENGINE
#define GLV_H_ENGINE

#include <iostream>
#include <stdarg.h>
#include <vector>

#include "gld_types.h"
#include "glv_FieldManager.h"
#include "glv_ProfileSwitcher.h"
#include "glv_ConfigMode.h"
#include "glv_ViewMode.h"
#include "glv_MenuBar.h"
#include "glv_ProfileEdit.h"



class Engine {
public:	
	ViewModeControl *viewModeCtrl;
	FieldManager *fieldManager;

	Engine();

	void init();
	void render();

	void onCanvasSizeChange(int width, int height);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);
	void onMouseMove(int x, int y);
	void onMouseWheel(signed short direction);
	void onKeyDown(char key);


private:
	ProfileEdit * profileEditCtrl;
	ProfileSwitcher *profiles;

	ConfigModeControl *configModeCtrl;
	MenuBar * menuBar;

	bool needToRender;

	RoomType actualRoom;

	void onRoomChange(void);
	void onProfileChange(void);
};

#endif
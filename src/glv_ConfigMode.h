#pragma once
#ifndef GLV_H_CONFIG_CONTROL
#define GLV_H_CONFIG_CONTROL

#include <fstream>
#include <string>
#include <gl/glew.h>

#include "glv_Constants.h"
#include "gld_types.h"
#include "gld_drawing.h"
#include "glv_Interface.h"

#include "glv_FieldManager.h"


class ConfigModeControl {
public:
	ConfigModeControl(FieldManager * fManager);
	void init();
	void render();
	void mouseDown(mouseButton button);
	void mouseUp(mouseButton button);
	//bool save();
	//bool load();

private:
	FieldManager *fieldManager;

	Interface::Menu popupDefault, popupOnField;
	Point mouseClickPosition, movedFieldPosition;
	bool addingField;
	bool movingField;
	bool resizingField;
	int selectedField;

	char * fieldTypeName(const int &type, const bool abriviation = false);
	Color * fieldTypeColor(const int &type);

	bool trySelectFieldUnderMouse();
	bool trySelectFieldHandleUnderMouse();
	
	int snappedToGrid(int val);
	Point snappedToGrid(const Point &point);
	
	void renderBackground();

	void renderField(const int i, bool selected);

	void onMenuClick(int itemID, std::string itemCaption);
	
};

#endif
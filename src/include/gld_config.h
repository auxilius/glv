#ifndef H_CONFIGURATOR
#define H_CONFIGURATOR

#include <fstream>
#include <string>
#include <gl/glew.h>

#include "gld_constants.h"
#include "gld_types.h"
#include "gld_drawing.h"
#include "gld_interface.h"


struct ConfigField {
public:
	unsigned visualizationType;
	Box border;
	ConfigField(Box cBorder);
	ConfigField(int x1, int y1, int x2, int y2);
	void moveScaleHandle(Point point);
	void moveScaleHandle(int x, int y);
	void makeSquare();
	void draw(bool selected);
	void drawScaleHandle();
	void drawText();
};

class gldConfigurator {
private:
	PopupMenu popupDefault, popupOnField;
	Point mouseClickPosition, movedFieldPosition;
	bool addingField;
	bool movingField;
	bool resizingField;
	int selectedField;
	std::vector<ConfigField> field;
	bool addField();
	bool addField(int x1, int y1, int x2, int y2);
	bool addField(Box border);
	void deleteField(int which);
	void deleteAllFields();
	unsigned fieldCount();
	bool selectFieldUnderMouse();
	bool selectFieldHandleUnderMouse();
	void loadPopupMenus();
	int snappedToGrid(int val);
	void renderBackground();
public:
	void init();
	void render();
	void mouseDown(mouseButton button);
	void mouseUp(mouseButton button);
	unsigned numberOfField();
	bool save();
	bool load();
};

#endif
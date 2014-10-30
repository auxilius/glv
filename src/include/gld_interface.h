#ifndef H_GLD_INTERFACE
#define H_GLD_INTERFACE

#include <string>
#include <vector>
#include <array>
#include <gl/glew.h>

#include "gld_constants.h"
#include "gld_types.h"
#include "gld_drawing.h"

enum ButtonColorRelation {
	BACK_NORMAL, BACK_HOVERED, BACK_CLICKED,
	BORDER_NORMAL, BORDER_HOVERED, BORDER_CLICKED,
	TEXT_NORMAL, TEXT_HOVERED, TEXT_CLICKED
};

class Button {
protected:
	double fontSize;
	Box border;
	char * textOnButton;
	std::array<GLfloat*,9> color;
	void initializeColors();
public:
	GLfloat borderWidth;
	Button();
	void draw();
	bool isClicked();
	bool isSeparator();
	void removeColor(ButtonColorRelation relation);
	void setColor(ButtonColorRelation relation, GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void setColor(ButtonColorRelation relation, GLfloat * pColor);
	void setFont(double size);
	// CAPTION
	char * getCaption() { return textOnButton; };
	void	setCaption(const char * newCaption);
	__declspec(property(get = getCaption, put = setCaption)) char * caption;
	// POSITION
	void setPosition(int x, int y) { border.moveTo(x, y); };
	Point getPosition() { return point(border.left, border.top); };
	__declspec(property(get = getPosition)) Point position;
	// WIDTH
	void setWidth(int newWidth) { border.setWidth(newWidth); };
	int getWidth() { return border.getWidth(); };
	__declspec(property(get = getWidth, put = setWidth)) int width;
	// HEIGHT
	void setHeight(int newHeight) { border.setHeight(newHeight); };
	int getHeight() { return border.getHeight(); };
	__declspec(property(get = getHeight, put = setHeight)) int height;
	void setSize(int w, int h) { setWidth(w); setHeight(h); };
};

class CheckButton : public Button {
	bool checked;
	bool firstClick;
	void processClick();
	void changeColors();
public:
	CheckButton();
	bool isChecked();
	void check(bool state = true);
	bool toggle();
};

#define POPUP_ACTIVE_BORDER_SIZE 5
#define POPUP_TIME_TO_HIDE 100
#define POPUPMENU_NULL ""

class PopupMenu {
protected:
	int minItemWidth;
	Box activeArea, border;
	std::vector<Button> item;
	bool active;
	void loadDefaultColors(Button * item);
public:
	bool checkable;
	PopupMenu();
	PopupMenu(std::vector<std::string> data);
	int addItem(const char * caption);
	void draw();
	bool isActive();
	void show(int x = -1, int y = -1, bool menuOverride = false);
	void hide();
	int selectedItemNumber();
	std::string selectedItemText();
};

class CheckPopupMenu : public PopupMenu {
private:
	std::vector<bool> optionChecked;
	void clearCheckedOptions();
	void loadCheckedColors(Button * item);
public:
	void setCheckedOptions(std::vector<unsigned> * checkedOptions);
	int checkedItem();
	int uncheckedItem();
};


class BottomMenuBar {

};

#endif
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
private:
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

#define POPUP_ACTIVE_BORDER_SIZE 5
#define POPUP_TIME_TO_HIDE 100
#define POPUPMENU_NULL ""

class PopupMenu {
private:
	int minItemWidth;
	Box activeArea, border;
	std::vector<Button> item;
	bool active;
public:
	PopupMenu();
	PopupMenu(std::vector<std::string> data);
	int addItem(const char * caption);
	void draw();
	bool isActive();
	void show(int x = -1, int y = -1);
	void hide();
	int selectedItemNumber();		// vrati hodnotu, podla toho ktora polozka bola kliknuta, ak nic, tak vrati 0
	std::string selectedItemText(); // vrati retazec z aktualne selectnutej polozky
};


#endif
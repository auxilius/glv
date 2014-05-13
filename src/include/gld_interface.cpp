#include "gld_interface.h"
#include <iostream>

using namespace std;

/************** BUTTON CLASS **************/

Button::Button() {
	initializeColors();
	setPosition(0, 0);
	fontSize = 9;
	setCaption("New Button");
	borderWidth = 1;
};
void Button::setCaption(const char * newCaption) { 
	int len = strlen(newCaption)+1;
	textOnButton = new char[len];
	strcpy_s(textOnButton, len, newCaption);
	setFontSize(fontSize);
	border.setWidth(getTextWidth(textOnButton) + 20);
	if (isSeparator())
		border.setHeight(10);	
	else
		border.setHeight(getTextHeight(textOnButton) + 12);
};
void Button::setFont(double size) {
	fontSize = size;
	setFontSize(fontSize);
	border.setWidth(getTextWidth(textOnButton) + 20);
};
void Button::draw(){
	glColor4fv(color[BACK_NORMAL]);
	if (!isSeparator()) {
		if (isClicked() && color[BACK_CLICKED] != NULL)
			glColor4fv(color[BACK_CLICKED]);
		else if (border.contains(input.mouse.x, input.mouse.y) && color[BACK_HOVERED] != NULL)
			glColor4fv(color[BACK_HOVERED]);
	}
	drawRect(GL_QUADS, border);

	if (borderWidth > 0) {
		glColor4fv(color[BORDER_NORMAL]);
		if (!isSeparator()) {
			if (isClicked() && color[BORDER_CLICKED] != NULL)
				glColor4fv(color[BORDER_CLICKED]);
			else if (border.contains(input.mouse.x, input.mouse.y) && color[BORDER_HOVERED] != NULL)
				glColor4fv(color[BORDER_HOVERED]);
		}
		glLineWidth(borderWidth);
		drawRect(GL_LINE_LOOP, border);
	}
	if (!isSeparator()) {
		glColor4fv(color[TEXT_NORMAL]);
		if (isClicked() && color[TEXT_CLICKED] != NULL)
			glColor4fv(color[TEXT_CLICKED]);
		else if (border.contains(input.mouse.x, input.mouse.y) && color[TEXT_HOVERED] != NULL)
			glColor4fv(color[TEXT_HOVERED]);
		setFontSize(fontSize);
		textPrint(border.left + 5, border.top + 5, textOnButton);
	}
	if (isSeparator()) {
		glColor4fv(color[BORDER_NORMAL]);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f((GLfloat)border.left, (GLfloat)border.center.y);
		glVertex2f((GLfloat)border.right, (GLfloat)border.center.y);
		glEnd();
	}
	
};
bool Button::isClicked() {
	return input.mousePressed(mbLeft) && border.contains(input.mouse.x, input.mouse.y);
};
bool Button::isSeparator() {
	return strlen(textOnButton) == 0;
};
void Button::initializeColors() {
	color.fill(NULL);
	setColor(BACK_NORMAL, 0.3f, 0.5f, 0.8f, 1.0f);
	setColor(BACK_CLICKED, 0.7f, 0.8f, 1.0f, 1.0f);
	setColor(BORDER_NORMAL, 0.4f, 0.6f, 0.8f, 1.0f);
	setColor(BORDER_HOVERED, 0.8f, 0.6f, 0.4f, 1.0f);
	setColor(TEXT_NORMAL, 0.0f, 0.0f, 0.0f, 1.0f);
};
void Button::removeColor(ButtonColorRelation relation) {
	if (relation == BACK_NORMAL || relation == BORDER_NORMAL || relation == TEXT_NORMAL) {
		setColor(relation, 0.0f, 0.0f, 0.0f, 0.0f);
	}
	else {
		if (color[relation] != NULL) 
			delete[] color[relation];
		color[relation] = NULL;
	}
};
void Button::setColor(ButtonColorRelation relation, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	if (color[relation] == NULL)
		color[relation] = new GLfloat[4];
	color[relation][0] = r;
	color[relation][1] = g;
	color[relation][2] = b;
	color[relation][3] = a;
}; 


/************** POPUP MENU CLASS **************/

PopupMenu::PopupMenu() {
	activeArea.set(-POPUP_ACTIVE_BORDER_SIZE, -POPUP_ACTIVE_BORDER_SIZE, POPUP_ACTIVE_BORDER_SIZE, POPUP_ACTIVE_BORDER_SIZE);
};

PopupMenu::PopupMenu(vector<string> data) {
	PopupMenu();
	for (unsigned i = 0; i < data.size(); i++) {
		addItem(data[i].c_str());
	}
};
int PopupMenu::addItem(const char * caption) {
	Button newItem;
	newItem.setCaption(caption);
	newItem.setPosition(0, activeArea.height);
	newItem.borderWidth = 0;
	newItem.setFont(9);
	if (newItem.width > minItemWidth) {
		minItemWidth = newItem.width;
		for (unsigned i = 0; i < item.size(); i++)
			item[i].width = minItemWidth;
		activeArea.width = minItemWidth + 2 * POPUP_ACTIVE_BORDER_SIZE;
	}
	else
		newItem.width = minItemWidth;
	activeArea.height = activeArea.height + newItem.height;
	newItem.removeColor(BORDER_HOVERED);
	newItem.setColor(BACK_NORMAL,  0.2f, 0.2f, 0.2f, 1.0f);
	newItem.setColor(BACK_HOVERED, 1.0f, 0.9f, 0.5f, 1.0f);
	newItem.setColor(BORDER_NORMAL, 0.0f, 0.0f, 0.0f, 0.20f);
	newItem.setColor(TEXT_NORMAL, 1.0f, 1.0f, 1.0f);
	newItem.setColor(TEXT_HOVERED, 0.2f, 0.15f, 0.05f);
	item.push_back(newItem);
	border.set(activeArea.left + POPUP_ACTIVE_BORDER_SIZE,
		activeArea.top + POPUP_ACTIVE_BORDER_SIZE,
		activeArea.right - POPUP_ACTIVE_BORDER_SIZE,
		activeArea.bottom - POPUP_ACTIVE_BORDER_SIZE);
	return item.size();
};
void PopupMenu::draw() {
	if (active) {
		for (unsigned i = 0; i < item.size(); i++)
			item[i].draw();
		
		glColor4f(0.0f,0.0f,0.0f, 0.25f);
		glLineWidth(2.0f);
		drawRect(GL_LINE_LOOP, border);

		if (!activeArea.contains(input.mouse))
			hide();
	}
};
void PopupMenu::show(int x, int y) {
	if (x == -1)
		x = input.mouse.x;
	if (y == -1)
		y = input.mouse.y;
	if (x > (int)canvas.width - activeArea.width + POPUP_ACTIVE_BORDER_SIZE * 2)
		x = canvas.width - activeArea.width + POPUP_ACTIVE_BORDER_SIZE * 2;
	if (y > (int)canvas.height - MENU_HEIGHT - activeArea.height + POPUP_ACTIVE_BORDER_SIZE * 2)
		y = canvas.height - MENU_HEIGHT - activeArea.height + POPUP_ACTIVE_BORDER_SIZE * 2;
	int ypos = y;
	for (unsigned i = 0; i < item.size(); i++) {
		item[i].setPosition(x, ypos);
		ypos += item[i].height;
	}
	activeArea.moveTo(x - 5, y - 5);
	border.moveTo(x, y);
	active = true;
};
void PopupMenu::hide() {
	active = false;
};
bool PopupMenu::isActive() {
	return active;
};
int PopupMenu::selectedItemNumber() {
	if (active) {
		for (unsigned i = 0; i < item.size(); i++) {
			if (item[i].isClicked()) {
				active = false;
				return i;
			}
		}
	}
	return -1;
};
std::string PopupMenu::selectedItemText() {
	int selectedNumber = selectedItemNumber();
	if (selectedNumber == -1)
		return POPUPMENU_NULL;
	else
		return item[selectedNumber].caption;
};


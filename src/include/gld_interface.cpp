#include "gld_interface.h"
#include <iostream>

using namespace std;

#define BUTTON_PADDING 5

/************** BUTTON CLASS **************/

Button::Button() {
	initializeColors();
	setPosition(0, 0);
	font = fontMain;
	setCaption("New Button");
	borderWidth = 1;
};
void Button::recalcSize() {
	border.setWidth(font->getWidth(textOnButton) + 2*BUTTON_PADDING);
	if (isSeparator())
		border.setHeight(2*BUTTON_PADDING);	
	else
		border.setHeight(font->getHeight() + 2*BUTTON_PADDING);
};
void Button::setCaption(const char * newCaption) { 
	int len = strlen(newCaption)+1;
	textOnButton = new char[len];
	strcpy_s(textOnButton, len, newCaption);
	recalcSize();
};
void Button::setFont(Font *f) {
	font = f;
	recalcSize();
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
		font->textOut(border.left + BUTTON_PADDING, border.top + ( border.height - (int)(font->getHeight()*0.7) ) / 2, textOnButton);
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
	setColor(BACK_NORMAL, 0.4f, 0.4f, 0.4f, 1.0f);
	setColor(BACK_HOVERED, 0.95f, 0.95f, 0.95f, 1.0f);
	setColor(BORDER_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f);
	setColor(TEXT_NORMAL, 1.0f, 1.0f, 1.0f);
	setColor(TEXT_HOVERED, 0.2f, 0.2f, 0.2f);
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
void Button::setColor(ButtonColorRelation relation, GLfloat * pColor) {
	if (color[relation] == NULL)
		color[relation] = new GLfloat[4];
	color[relation][0] = pColor[0];
	color[relation][1] = pColor[1];
	color[relation][2] = pColor[2];
	color[relation][3] = pColor[3];
};

/************** CHECKBUTTON CLASS **************/
CheckButton::CheckButton() {
	checked = false;
	firstClick = true;
};
void CheckButton::processClick() {
	if (isClicked()) {
		if (firstClick)
			toggle();
		firstClick = false;
	} else
		firstClick = true;
	changeColors();
};
void CheckButton::changeColors() {
	if (checked)
		setColor(BACK_NORMAL, 0.5f, 0.5f, 0.5f);
	else
		setColor(BACK_NORMAL, 0.1f, 0.1f, 0.1f);
};
bool CheckButton::isChecked() {
	return checked; 
};
void CheckButton::check(bool state) {
	checked = true; 
};
bool CheckButton::toggle() { 
	checked = !checked; 
	return checked; 
};



/************** POPUP MENU CLASS **************/

PopupMenu::PopupMenu() {
	active = false;
	activeArea.set(-POPUP_ACTIVE_BORDER_SIZE, -POPUP_ACTIVE_BORDER_SIZE, POPUP_ACTIVE_BORDER_SIZE, POPUP_ACTIVE_BORDER_SIZE);
};
PopupMenu::PopupMenu(vector<string> data) {
	PopupMenu();
	for (unsigned i = 0; i < data.size(); i++) {
		addItem(data[i].c_str());
	}
};
void PopupMenu::loadDefaultColors(Button * item) {
	item->removeColor(BORDER_HOVERED);
	item->removeColor(BACK_CLICKED);
	item->setColor(BACK_NORMAL, 0.2f, 0.2f, 0.2f, 1.0f);
	item->setColor(BACK_HOVERED, 0.85f, 0.8f, 0.5f, 1.0f);
	item->setColor(BORDER_NORMAL, 0.0f, 0.0f, 0.0f, 0.20f);
	item->setColor(TEXT_NORMAL, 1.0f, 1.0f, 1.0f);
	item->setColor(TEXT_HOVERED, 0.2f, 0.15f, 0.05f);
};
int PopupMenu::addItem(const char * caption) {
	Button newItem;
	newItem.setCaption(caption);
	newItem.setPosition(0, activeArea.height);
	newItem.borderWidth = 0;
	newItem.setFont(fontMain);
	if (newItem.width > minItemWidth) {
		minItemWidth = newItem.width;
		for (unsigned i = 0; i < item.size(); i++)
			item[i].width = minItemWidth;
		activeArea.width = minItemWidth + 2 * POPUP_ACTIVE_BORDER_SIZE;
	}
	else
		newItem.width = minItemWidth;
	activeArea.height = activeArea.height + newItem.height;
	loadDefaultColors(&newItem);
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
void PopupMenu::show(int x, int y, bool menuOverride) {
	if (x == -1)
		x = input.mouse.x;
	if (y == -1)
		y = input.mouse.y;
	int xMax = canvas.width - activeArea.width + POPUP_ACTIVE_BORDER_SIZE * 2;
	if (x > xMax)
		x = xMax;
	int yMax = canvas.height - activeArea.height + POPUP_ACTIVE_BORDER_SIZE * 2;
	if (!menuOverride)
		yMax -= MENU_HEIGHT;
	if (y > yMax)
		y = yMax;
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


/************** CHECKABLE POPUP MENU CLASS **************/

void CheckPopupMenu::loadCheckedColors(Button * item) {
	item->setColor(BACK_NORMAL, 0.2f, 0.3f, 0.2f, 1.0f);
	item->setColor(BACK_HOVERED, 9.0f, 1.0f, 0.55f, 1.0f);
};
void CheckPopupMenu::clearCheckedOptions() {
	if (optionChecked.size() != item.size())
		optionChecked.resize(item.size(), false);
	std::fill(optionChecked.begin(), optionChecked.end(), false);
	for (unsigned i = 0; i < item.size(); i++)
		loadDefaultColors(&item[i]);
};
void CheckPopupMenu::setCheckedOptions(std::vector<unsigned> * checkedOptions) {
	clearCheckedOptions();
	for (unsigned i = 0; i < checkedOptions->size(); i++) {
		unsigned optionNo = checkedOptions->operator[](i);
		if (optionNo < item.size()) {
			loadCheckedColors(&item[optionNo]);
			optionChecked[optionNo] = true;
		}
	}
};
int CheckPopupMenu::checkedItem() {
	if (!active) return -1;
	for (unsigned i = 0; i < item.size(); i++)
		if (item[i].isClicked() && !optionChecked[i]) {
			loadCheckedColors(&item[i]);
			optionChecked[i] = true;
			return i;
		}
	return -1;
};
int CheckPopupMenu::uncheckedItem() {
	if (!active) return -1;
	for (unsigned i = 0; i < item.size(); i++)
	if (item[i].isClicked() && optionChecked[i]) {
		loadDefaultColors(&item[i]);
		optionChecked[i] = false;
		return i;
	}
	return -1;
};



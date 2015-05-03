#include "glv_Interface.h"
#include <iostream>
#include <algorithm>


//   CONSTANTS   //
#define ACTIVE_BORDER_SIZE	5
#define TIME_TO_HIDE		100
#define SEPARATOR_HRIGHT	10
#define ITEM_SPACING		1


using namespace Interface;

/**** POP-MENU CLASS ****/

Menu::Menu() {
	active = false;
	activeArea.set(-ACTIVE_BORDER_SIZE, -ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE);
};

Menu::Menu(std::vector<std::string> data) {
	active = false;
	activeArea.set(-ACTIVE_BORDER_SIZE, -ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE);
	for (unsigned i = 0; i < data.size(); i++) {
		addItem(data[i].c_str());
	}
};

Button Menu::newItem(std::string name) {
	Button item;
	item.setCaption(name);
	item.borderWidth = 0;
	item.setFont(fontMain);
	return item;
};

int Menu::addItem(std::string caption) {
	Button item = newItem(caption);
	item.setPosition(0, activeArea.height);
	if (item.width > minItemWidth) {
		minItemWidth = item.width;
		for (unsigned i = 0; i < this->item.size(); i++)
			this->item[i].width = minItemWidth;
		activeArea.width = minItemWidth + 2 * ACTIVE_BORDER_SIZE;
	}
	else
		item.width = minItemWidth;
	if (item.caption == NULL_ITEM)
		item.height = SEPARATOR_HRIGHT;
	activeArea.height = activeArea.height + item.height + ITEM_SPACING;
	this->item.push_back(item);
	border.set(activeArea.left + ACTIVE_BORDER_SIZE,
		activeArea.top + ACTIVE_BORDER_SIZE,
		activeArea.right - ACTIVE_BORDER_SIZE,
		activeArea.bottom - ACTIVE_BORDER_SIZE);
	return this->item.size()-1;
};

void Menu::clearAllItems() {
	item.clear();
	active = false;
	activeArea.set(-ACTIVE_BORDER_SIZE, -ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE, ACTIVE_BORDER_SIZE);
	minItemWidth = 0;
};

void Menu::draw(bool highlightHovered) {
	if (active) {

		glLineWidth(1.0f);
		drawBox(&border, clBtnBack, true);
		
		for (unsigned i = 0; i < item.size(); i++) {
			if ( item[i].caption == NULL_ITEM ) {
				item[i].draw( false );
				unsigned yc = item[i].position.y + item[i].height / 2;
				Box line(item[i].position.x, yc, item[i].position.x+item[i].width, yc);
				drawBox(&line, clBtnBorder, false);
			} else {
				item[i].draw( true );
			}
		}

		drawBox(&border, clBtnBorder, false);

		if (!activeArea.contains(input.mouse))
			hide();
	}
};

void Menu::show(int x, int y) {
	if (x == -1)
		x = input.mouse.x;
	if (y == -1)
		y = input.mouse.y;
	int xMax = canvas.width - activeArea.width + ACTIVE_BORDER_SIZE * 2;
	if (x > xMax)
		x = xMax;
	int yMax = canvas.height - activeArea.height + ACTIVE_BORDER_SIZE * 2;
	if (y > yMax)
		y = yMax;
	int ypos = y;
	for (unsigned i = 0; i < item.size(); i++) {
		item[i].setPosition(x, ypos);
		ypos += item[i].height + ITEM_SPACING;
	}
	activeArea.moveTo(x - ACTIVE_BORDER_SIZE, y - ACTIVE_BORDER_SIZE);
	border.moveTo(x, y);
	active = true;
};

void Menu::showAtMousePosition() {
	
	int x = input.mouse.x;
	int y = input.mouse.y;
	
	int xMax = canvas.width - activeArea.width + ACTIVE_BORDER_SIZE * 2;

	if (x > xMax)
		x = xMax;
	
	int yMax = canvas.height - activeArea.height + ACTIVE_BORDER_SIZE * 2;
	
	if (y > yMax)
		y = yMax;
	
	activeArea.moveTo(x - ACTIVE_BORDER_SIZE, y - ACTIVE_BORDER_SIZE);
	border.moveTo(x, y);

	// each individual item move under each other
	for (unsigned i = 0; i < item.size(); i++) {
		item[i].setPosition(x, y);
		y += item[i].height + ITEM_SPACING;
	}
		
	active = true;
};



void Menu::hide() {
	active = false;
};

bool Menu::isActive() {
	return active;
};

int Menu::getWidth() {
	return minItemWidth;
};

std::string Menu::getItemCaption(int itemID) {
	if (itemID >= 0 && itemID < (int)item.size())
		return item[itemID].caption;
	return "";
};

void Menu::onMouseDown(mouseButton button) {
	if (active) {
		for (unsigned i = 0; i < item.size(); i++) {
			if (item[i].caption == NULL_ITEM)
				continue;
			item[i].onMouseDown(button);
			if (OnItemClick && item[i].isHovered()) {
				OnItemClick(i, item[i].caption);
				return;
			}
		}
	}
};


/**** CHECKABLE POP-MENU CLASS ****/

Button CheckMenu::newItem(std::string name) {
	CheckButton item;
	item.setCaption(name);
	item.borderWidth = 0;
	item.setFont(fontMain);
	return item;
};

void CheckMenu::setCheckedOptions(std::vector<unsigned> * checkedOptions) {
	for (unsigned i = 0; i < item.size(); i++) {
		CheckButton* btn = static_cast<CheckButton*>(&item[i]);
		btn->checked = std::find(checkedOptions->begin(), checkedOptions->end(), i) != checkedOptions->end();
	}
};

void CheckMenu::onMouseDown(mouseButton button) {
	if (active) {
		for (unsigned i = 0; i < item.size(); i++) {
			CheckButton* btn = static_cast<CheckButton*>(&item[i]);
			btn->onMouseDown(button);
			if (OnItemChange && btn->isHovered()) {
				OnItemChange(i, btn->caption, btn->checked);
			}
		}
	}
};



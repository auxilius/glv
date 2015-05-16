#include "glv_Interface.h"
#include <iostream>
#include <algorithm>


//   CONSTANTS   //
#define SEPARATOR_HEIGHT	10
#define ITEM_SPACING		1


using namespace Interface;

/**** POP-MENU CLASS ****/

Menu::Menu() {
	visible = false;
};

Button Menu::createItem(std::string name) {
	Button item;
	item.setCaption(name);
	item.outlineWidth = 0;
	item.setFont(fontMain);
	return item;
};

int Menu::addItem(std::string caption) {
	// create new item with the caption
	Button newItem = createItem(caption);
	newItem.setPosition(border.left, border.bottom);
	// if new item width is bigger, 
	// resize all existing items to this new width
	if (newItem.width > border.width) {
		border.width = newItem.width;
		for (unsigned i = 0; i < this->item.size(); i++)
			this->item[i].width = border.width;
		
	} // if it is smaller, resize new item to the width of menu
	else
		newItem.width = border.width;
	// check separator option 
	if (newItem.caption == NULL_ITEM)
		newItem.height = SEPARATOR_HEIGHT;
	// set border height
	border.height = border.height + newItem.height + ITEM_SPACING;
	// add new item to the list of items
	this->item.push_back(newItem);
	// return id of last item
	return this->item.size()-1;
};

std::string Menu::getItemCaption(int itemID) {
	if (itemID >= 0 && itemID < (int)item.size())
		return item[itemID].caption;
	return NULL_ITEM;
};

int Menu::getItemID(std::string caption) {
	for (unsigned i = 0; i < item.size(); i++) {
		if (item[i].caption == caption)
			return i;
	}
	return -1;
};

unsigned Menu::getItemCount() {
	return item.size();
};

void Menu::clearAllItems() {
	item.clear();
	border.setSize(0,0);
};

void Menu::onRender(bool highlightHovered) {
	// halt if list is not active
	if (!visible)
		return;

	// draw solid background rectangle
	drawBox(&border, clBtnBack, true);
	
	// render individual items
	for (unsigned i = 0; i < item.size(); i++) {
		// item is separator
		if ( item[i].caption == NULL_ITEM ) {
			item[i].onRender( false );
			unsigned yc = item[i].position.y + item[i].height / 2;
			Box line(item[i].position.x, yc, item[i].position.x+item[i].width, yc);
			drawBox(&line, clBtnBorder, false);
		} 
		// item is button
		else {
			item[i].onRender( true );
		}
	}

	// draw border
	glLineWidth(outlineWidth);
	drawBox(&border, clBtnBorder, false);
};

void Menu::show(int x, int y) {
	// check if the menu will not be shown outside of canvas
	int xMax = canvas.width - border.width;
	if (x > xMax)
		x = xMax;
	int yMax = canvas.height - border.height;
	if (y > yMax)
		y = yMax;
	// move border to the showing position
	border.moveTo(x, y);
	// each individual item move under each other
	for (unsigned i = 0; i < item.size(); i++) {
		item[i].setPosition(x, y);
		y += item[i].height + ITEM_SPACING;
	}
	visible = true;
};

void Menu::showAtMousePosition() {
	show( input.mouse.x, input.mouse.y);
};

void Menu::hide() {
	visible = false;
};


void Menu::onMouseDown(mouseButton button) {
	onMouseDownGetID(button);
};

int Menu::onMouseDownGetID(mouseButton button) {
	if (!visible)
		return -1;
	visible = false;
	for (unsigned i = 0; i < item.size(); i++) {
		// skip, if item is just separator
		if (item[i].caption == NULL_ITEM)
			continue;
		// call event and callback
		//item[i].onMouseDown(button);
		if (item[i].isHovered()) {
			if (callOnItemClick)
				callOnItemClick(i, item[i].caption);
			return i;
		}
	}
	return -1;
};


/**** CHECKABLE POP-MENU CLASS ****/

Button CheckMenu::createItem(std::string name) {
	CheckButton item;
	item.setCaption(name);
	item.outlineWidth = 0;
	item.setFont(fontMain);
	item.checked = false;
	return item;
};

void CheckMenu::setCheckedOptions(std::vector<unsigned> * checkedOptions) {
	for (unsigned i = 0; i < item.size(); i++) {
		CheckButton* btn = static_cast<CheckButton*>(&item[i]);
		btn->checked = std::find(checkedOptions->begin(), checkedOptions->end(), i) != checkedOptions->end();
	}
};

void CheckMenu::onMouseDown(mouseButton button) {
	if (!visible)
		return;
	for (unsigned i = 0; i < item.size(); i++) {
		CheckButton* btn = static_cast<CheckButton*>(&item[i]);
		btn->onMouseDown(button);
		if (btn->isHovered() && callOnItemChange) {
			callOnItemChange(i, btn->caption, btn->checked);
			return;
		}
	}
	visible = false;
};



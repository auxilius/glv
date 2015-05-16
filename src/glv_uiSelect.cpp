#include "glv_Interface.h"


using namespace Interface;


Select::Select() {
	actualSelected = -1;
	caption = Interface::NULL_ITEM;
};

int Select::addItem(std::string caption) {
	int newItem = menuList.addItem(caption);
	if (actualSelected==-1)
		setSelected(newItem);
	return newItem;
};

void Select::clearAllItems() {
	menuList.clearAllItems();
	actualSelected = -1;
	caption = NULL_ITEM;
};

void Select::selectItem(std::string item) {
	int id = menuList.getItemID(item);
	if (id != -1)
		setSelected(id);
};

void Select::selectItem(const int itemID) {
	if (itemID >= 0 && itemID < (int)menuList.getItemCount())
		setSelected(itemID);
};

void Select::setSelected(int id) {
	if (id != actualSelected) {
		caption = menuList.getItemCaption(id);
		actualSelected = id;
		border.width = menuList.getWidth();
	}
};

void Select::onMouseDown(mouseButton button) {
	if (!visible)
		return;
	if (isOpened()) {
		int id = menuList.onMouseDownGetID(button);
		if (id!=-1) {
			menuList.hide();
			setSelected(id);	
			if (callOnChange)
				callOnChange(id, caption);
		}
	}
	else {
		if (isHovered()) {
			menuList.show(border.left, border.top);
			if (callOnClick)
				callOnClick(button);
		}
	}

};

void Select::onRender() {
	if (!visible)
		return;
	Button::onRender(true);
	menuList.onRender(true);
};

bool Select::isOpened() {
	return menuList.visible;
};
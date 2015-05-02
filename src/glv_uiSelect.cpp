#include "glv_Interface.h"


using namespace Interface;


Select::Select() {
	actualSelected = -1;
	caption = NULL_ITEM;
	callOnClick = std::bind(&Select::onClick, this);
	using namespace std::placeholders;
	menuList.callOnItemClick = std::bind(&Select::onItemClick, this, _1, _2);
};

int Select::addItem(std::string caption) {
	int newItem = menuList.addItem(caption);
	selectItem(caption);
	return newItem;
};

void Select::clearAllItems() {
	menuList.clearAllItems();
	actualSelected = -1;
	caption = NULL_ITEM;
};

void Select::selectItem(std::string item) {
	caption = item;
	border.width = menuList.getWidth();
};

void Select::onMouseDown(mouseButton button) {
	if (isOpened())
		menuList.onMouseDown(button);
	else
		Button::onMouseDown(button);
};

void Select::onRender() {
	Button::onRender(true);
	menuList.onRender(true);
};

void Select::onClick() {
	menuList.show(border.left, border.top);
};

void Select::onItemClick(int id, std::string caption) {
	selectItem(caption);	
	menuList.hide();
	if (callOnChange)
		callOnChange(id, caption);
};

bool Select::isOpened() {
	return menuList.visible;
};
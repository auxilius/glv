#include "glv_Interface.h"


using namespace Interface;


Select::Select() {
	actualSelected = -1;
	textOnButton = "";
	OnClick = std::bind(&Select::onClickButton, this);
	using namespace std::placeholders;
	menuList.OnItemClick = std::bind(&Select::onItemClick, this, _1, _2);
};

int Select::addItem(std::string caption) {
	int newItem = menuList.addItem(caption);
	select(caption);
	border.width = menuList.getWidth();
	return newItem;
};

void Select::clearAllItems() {
	menuList.clearAllItems();
	actualSelected = -1;
	textOnButton = "";
};

void Select::select(std::string item) {
	textOnButton = item;
};

void Select::onMouseDown(mouseButton button) {
	if (menuList.isActive())
		menuList.onMouseDown(button);
	else
		Button::onMouseDown(button);
};

void Select::draw() {
	Button::draw(true);
	if (menuList.isActive())
		menuList.draw(true);
};

void Select::onClickButton() {
	menuList.show(border.left, border.top);
};

void Select::onItemClick(int id, std::string caption) {
	select(caption);	
	menuList.hide();
	if (OnChange)
		OnChange(id, caption);
};

bool Select::isActive() {
	return menuList.isActive();
};
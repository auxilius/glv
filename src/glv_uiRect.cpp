#include "glv_Interface.h"

using namespace Interface;

/****   Rect   ****/

Rect::Rect() {
	loadDefaultColors();
	setPosition(0, 0);
	outlineWidth = 1;
};

void Rect::onRender(bool alsoHoverColors) {
	if ( alsoHoverColors && isHovered() )
		drawBox(&border, *colorBackHover);
	else
		drawBox(&border, *colorBack);

	if (outlineWidth >= 1)
		drawBorder(alsoHoverColors);
};

void Rect::drawBorder(bool alsoHoverAndClick) {
	glLineWidth(outlineWidth);
	if ( alsoHoverAndClick && isHovered() )
		drawBox(&border, *colorBorderHover, false);
	else
		drawBox(&border, *colorBorder, false);
};

bool Rect::isHovered() {
	return border.contains(input.mouse);
};

void Rect::loadDefaultColors() {
	colorBack = &clBtnBack;
	colorBackHover = &clBtnBackHover;
	colorBorder = &clBtnBorder;
	colorBorderHover = &clBtnBorderHover;
};

void Rect::onMouseDown(mouseButton button) {
	if (isHovered() && callOnClick) {
		callOnClick(button);
	}
};

// POSITION
void Rect::setPosition(int x, int y) { 
	border.moveTo(x, y); 
};
Point Rect::getPosition() { 
	return point(border.left, border.top); 
};
// WIDTH
void Rect::setWidth(int newWidth) { 
	border.setWidth(newWidth); 
};
int Rect::getWidth() { 
	return border.getWidth(); 
};
// HEIGHT
void Rect::setHeight(int newHeight) { 
	border.setHeight(newHeight); 
};
int Rect::getHeight() { 
	return border.getHeight(); 
};
// SIZE (WIDTH + HEIGHT)
void Rect::setSize(int w, int h) { 
	setWidth(w); 
	setHeight(h); 
};
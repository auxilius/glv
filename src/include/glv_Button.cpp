#include "glv_Interface.h"
#include <iostream>

using namespace Interface;

//   CONSTANTS   //
#define DEFAULT_CAPTION	"New Button"
#define TEXT_PADDING	5


/****   BUTTON   ****/

Button::Button() {
	loadDefaultColors();
	setPosition(0, 0);
	font = fontMain;
	setCaption(DEFAULT_CAPTION);
	borderWidth = 1;
};

void Button::recalcSize() {
	border.setWidth(font->getWidth(textOnButton) + 2*TEXT_PADDING);
	if (font == NULL)
		border.setHeight(2*TEXT_PADDING);	
	else
		border.setHeight(font->getHeight() + 2*TEXT_PADDING);
};

void Button::setCaption(std::string newCaption) { 
	textOnButton = newCaption;
	recalcSize();
};

void Button::setFont(Font *f) {
	font = f;
	recalcSize();
};

void Button::draw(bool alsoHoverColors) {
	if ( alsoHoverColors && isHovered() )
		drawBox(&border, *color.hoverBack);
	else
		drawBox(&border, *color.back);

	if (borderWidth > 0)
		drawBorder(alsoHoverColors);

	if (caption != "")
		drawText(alsoHoverColors);
};

void Button::drawBorder(bool alsoHoverAndClick) {
	glLineWidth(borderWidth);
	if ( alsoHoverAndClick && isHovered() )
		drawBox(&border, *color.hoverBorder, false);
	else
		drawBox(&border, *color.border, false);
};

void Button::drawText(bool alsoHoverAndClick) {
	if ( alsoHoverAndClick && isHovered() )
		glColor4fv(*color.hoverText);
	else
		glColor4fv(*color.text);
	font->textOut(border.left + TEXT_PADDING, border.top + ( border.height - (int)(font->getHeight()*0.7) ) / 2, textOnButton);
};

bool Button::isHovered() {
	return border.contains(input.mouse);
};

void Button::loadDefaultColors() {
	color.back = &clBtnBack;
	color.hoverBack = &clBtnBackHover;
	color.border = &clBtnBorder;
	color.hoverBorder = &clBtnBorderHover;
	color.text = &clBtnText;
	color.hoverText = &clBtnTextHover;
};

void Button::onMouseDown(mouseButton button) {
	if (isHovered() && OnClick) {
		OnClick();
	}
};


/****   CHECK-BUTTON   ****/

CheckButton::CheckButton() {
	checked = false;
};

void CheckButton::setChecked(bool state) {
	checkState = state;
	if (checked) {
		color.back = &clBtnBackChecked;
		color.hoverBack = &clBtnBackChecked;
	}
	else {
		color.back = &clBtnBack;
		color.hoverBack = &clBtnBackHover;
	}
};

void CheckButton::onMouseDown(mouseButton button) {
	if (isHovered()) {
		setChecked(!checkState);
		if (OnChange)
			OnChange(checkState);
	}
};


#include "glv_Interface.h"
#include <iostream>

using namespace Interface;

//   CONSTANTS   //
#define DEFAULT_CAPTION	"New Button"
#define TEXT_PADDING	5


/****   BUTTON   ****/

Button::Button() {
	loadDefaultColors();
	myFont = fontMain;
	setCaption(DEFAULT_CAPTION);
};

void Button::fitSizeToText() {
	border.setWidth(myFont->getWidth(textOnButton) + 2*TEXT_PADDING);
	if (myFont == NULL)
		border.setHeight(2*TEXT_PADDING);	
	else
		border.setHeight(myFont->getHeight() + 2*TEXT_PADDING);
};

void Button::onRender(bool alsoHoverColors) {
	Rect::onRender(alsoHoverColors);
	if (caption != "")
		drawText(alsoHoverColors);
};

void Button::drawText(bool alsoHoverAndClick) {
	if ( alsoHoverAndClick && isHovered() )
		glColor4fv(*colorTextHover);
	else
		glColor4fv(*colorText);
	myFont->textOut(border.left + TEXT_PADDING, border.top + ( border.height - (int)(myFont->getHeight()*0.7) ) / 2, textOnButton);
};


void Button::loadDefaultColors() {
	Rect::loadDefaultColors();
	colorText = &clBtnText;
	colorTextHover = &clBtnTextHover;
};

// FONT
Font* Button::getFont() { 
	return myFont; 
};
void Button::setFont(Font *newFont) {
	myFont = newFont;
	fitSizeToText();
};

// CAPTION
std::string Button::getCaption() { 
	return textOnButton; 
};
void Button::setCaption(std::string newCaption) { 
	textOnButton = newCaption;
	fitSizeToText();
};


/****   CHECK-BUTTON   ****/

CheckButton::CheckButton() {
	checked = false;
};

bool CheckButton::getChecked() { 
	return checkState; 
};
void CheckButton::setChecked(bool state) {
	checkState = state;
	if (checkState) {
		colorBack = &clBtnBackChecked;
		colorBackHover = &clBtnBackChecked;
	}
	else {
		Button::loadDefaultColors();
	}
};

void CheckButton::onMouseDown(mouseButton button) {
	Button::onMouseDown(button);
	if (isHovered()) {
		setChecked(!checkState);
		if (callOnChange)
			callOnChange(checkState);
	}
};


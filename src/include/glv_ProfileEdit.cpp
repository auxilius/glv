#include "glv_ProfileEdit.h"



ProfileEdit::ProfileEdit(ProfileSwitcher *pSwitch) {
	// handle to the Profile Switcher
	profiles = pSwitch;
	// initialize values
	renaming = false;
	profileBeingEdited = -1;
	active = false;
	// initialize interface
	editText = "";

	finish.caption = "OK";
	finish.OnClick = std::bind(&ProfileEdit::onConfirm, this);

	cancel.caption = "Cancel";
	cancel.OnClick = std::bind(&ProfileEdit::onCancel, this);

	editBorder.setSize(250, fontMain->getSize()+10);
};

bool ProfileEdit::isActive() {
	return active;
};

void ProfileEdit::startRenaming(int profileId) {
	active = true;
	renaming = true;
	profileBeingEdited = profileId;	
	editText = profiles->getName(profileId);
	recalculatePositions();
};

void ProfileEdit::startAddingNew() {
	active = true;
	renaming = false;
	profileBeingEdited = -1;
	editText = "";
	recalculatePositions();
};

void ProfileEdit::recalculatePositions() {
	// beging of the interface
	int x = (canvas.width - 250) / 2;
	int y = canvas.height / 2 - 25;
	editBorder.moveTo(x, y);
	// calculate width both buttons placed beside each other
	int buttonsWidth = finish.width + cancel.width + 10;
	// set the coordinates to the finish button position
	x = (canvas.width - buttonsWidth) / 2;
	y += editBorder.height + 10;
	finish.setPosition(x, y);
	// move the coordinates to the cancel button position
	x += finish.width + 10;
	cancel.setPosition(x, y);
};

void ProfileEdit::render() {
	if (!active) return;
	// create the caption
	std::string caption =  "Name of new profile";
	if (renaming)
		caption = "Rename: "+profiles->getName(profileBeingEdited);
	// draw the caption
	glColor(150, 150, 150);
	fontLarge->textOut(canvas.width/2, editBorder.top - fontLarge->getSize() - 10, caption, faCenter);
	// draw edit box
	drawBox(&editBorder, clGray40);
	// draw edit box text
	glColor(200, 200, 200);
	fontMain->textOut(editBorder.left + 10, editBorder.top + 5, editText);
	// render buttons
	finish.draw();
	cancel.draw();
};

void ProfileEdit::mouseDown(mouseButton button) {
	finish.onMouseDown(button);
	cancel.onMouseDown(button);
};

void ProfileEdit::keyDown(char key) {
	// BACKSPACE pressed
	if (key == KEY_BACKSPACE) {
		if (editText.length() > 0)
			editText.erase(editText.end()-1);
	}
	else 
	// ENTER key pressed
	if (key == KEY_ENTER) {
		onConfirm();
	}
	else 
	// ESCAPE key
	if (key == KEY_ESCAPE) { 
		onCancel();
	}
	else 
	// text edit key input	
	if ( ('a'<=key && key<='z') || ('A'<=key && key<='Z') || ('0'<=key && key<='9') || key==KEY_SPACE ) {
		if (editText.length() < 23)
			editText += key;
	}
};

void ProfileEdit::onConfirm(void) {
	if (renaming)
		profiles->rename(profileBeingEdited, editText);
	else
		profiles->add(editText);
	active = false;
};

void ProfileEdit::onCancel(void) {
	active = false;
};

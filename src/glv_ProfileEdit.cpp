#include "glv_ProfileEdit.h"



ProfileEdit::ProfileEdit(ProfileSwitcher *pSwitch) {
	// handle to the Profile Switcher
	profiles = pSwitch;
	// initialize values
	editing = false;
	profileBeingEdited = -1;
	active = false;
	// initialize interface
	editText = "";

	btnFinish.caption = "OK";
	btnFinish.callOnClick = std::bind(&ProfileEdit::onConfirm, this);

	btnCancel.caption = "Cancel";
	btnCancel.callOnClick = std::bind(&ProfileEdit::onCancel, this);

	btnRemove.caption = "Delete";
	btnRemove.callOnClick = std::bind(&ProfileEdit::onRemove, this);

	editBorder.setSize(250, fontMain->getSize()+10);
};

bool ProfileEdit::isActive() {
	return active;
};

void ProfileEdit::startEditing(int profileId) {
	active = true;
	editing = true;
	profileBeingEdited = profileId;	
	editText = profiles->getName(profileId);
	recalculatePositions();
};

void ProfileEdit::startAddingNew() {
	active = true;
	editing = false;
	profileBeingEdited = -1;
	editText = "";
	recalculatePositions();
};

void ProfileEdit::recalculatePositions() {
	// beging of the interface
	int y = canvas.height / 2 - 25;
	int x = (canvas.width - 250) / 2;
	// name edit box
	editBorder.moveTo(x, y);
	// width of both buttons beside each other
	int buttonsWidth = btnFinish.width + btnCancel.width + 10;
	// set the coordinates to the btnFinish button position
	x = editBorder.right - buttonsWidth;
	y += editBorder.height + 10;
	// button positions
	btnRemove.setPosition(editBorder.left+1, y);
	btnFinish.setPosition(x, y);
	x += btnFinish.width + 10;
	btnCancel.setPosition(x, y);
};

void ProfileEdit::render() {
	if (!active) return;
	// create the caption
	std::string caption =  "Name of new profile";
	if (editing)
		caption = "Edit: " + profiles->getName(profileBeingEdited);
	// draw the caption
	glColor(150, 150, 150);
	fontLarge->textOut(canvas.width/2, editBorder.top - fontLarge->getSize() - 10, caption, faCenter);
	// draw edit box
	drawBox(&editBorder, clGray40);
	// draw edit box text
	glColor(200, 200, 200);
	fontMain->textOut(editBorder.left + 10, editBorder.top + 5, editText + "_");
	// render buttons
	btnFinish.onRender();
	btnCancel.onRender();
	if (editing)
		btnRemove.onRender();
};

void ProfileEdit::mouseDown(mouseButton button) {
	btnFinish.onMouseDown(button);
	btnCancel.onMouseDown(button);
	btnRemove.onMouseDown(button);
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
	if (editing)
		profiles->rename(profileBeingEdited, editText);
	else
		profiles->add(editText);
	active = false;
};

void ProfileEdit::onCancel(void) {
	active = false;
};

void ProfileEdit::onRemove(void) {
	if (editing) {
		profiles->remove(profileBeingEdited);
		active = false;
	}
};

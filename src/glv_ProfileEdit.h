#pragma once
#ifndef GLV_H_PROFILE_EDIT
#define GLV_H_PROFILE_EDIT

#include <string>
#include "gld_constants.h"
#include "glv_ProfileSwitcher.h"
#include "glv_Interface.h"

class ProfileEdit {

public:
	ProfileEdit(ProfileSwitcher *pSwitch);

	bool isActive();
	void startRenaming(int profileId);
	void startAddingNew();

	void recalculatePositions();
	void render();
	void mouseDown(mouseButton button);
	void keyDown(char key);

private:
	ProfileSwitcher * profiles;
	
	bool active;
	bool renaming;
	int profileBeingEdited;
	
	std::string editText;
	Box editBorder;
	Interface::Button finish, cancel;

	void onConfirm(void);
	void onCancel(void);
	
};



#endif
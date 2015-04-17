#pragma once
#ifndef GLV_H_MENU_BAR
#define GLV_H_MENU_BAR

#include "gld_types.h"
#include "glv_Interface.h"
#include "glv_ProfileSwitcher.h"
#include "glv_ProfileEdit.h"

typedef std::function<void(void)> EventCallback;

class MenuBar {
public:
	EventCallback funcRoomChange;
	EventCallback funcProfileChange;

	MenuBar(ProfileSwitcher *pSwitcher, ProfileEdit *pEdit);
	bool isUnderCursor();
	void init();
	void reloadProfileList();
	void update(const RoomType &room);
	void onRender();
	void onMouseDown(mouseButton button);

private:
	int markedProfile;

	ProfileSwitcher * profiles;
	ProfileEdit		* profileEditor;

	Interface::Button btnRoomSwitch, btnProfileSelect;
	Interface::PopupMenu menuProfileSelect, menuProfileEdit;

	Box getBorder();
	void update();

	void onRoomButtonClick();
	void onProfileButtonClick();
	void onMenuProfilesClick(int itemID, std::string itemName);
	void onMenuProfileEditClick(int itemID, std::string itemName);
	
};


#endif
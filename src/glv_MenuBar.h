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
	// called when the user changes room
	EventCallback callRoomChange;
	// called when the user changes profile
	EventCallback callProfileChange;
	// constructor
	MenuBar(ProfileSwitcher *pSwitcher, ProfileEdit *pEdit);
	// initialize
	void init(const RoomType &room);
	// chceck if mouse is over some menu element
	bool isUnderCursor();
	// rendering of menu bar
	void onRender();
	// mouse action was done
	void onMouseDown(mouseButton button);
	// canvas changed it's size
	void onCanvasSizeChange(int width, int height);
	// profile edit screen was closed
	void onProfileEditClose(const RoomType &room);
	// room was changed
	void onRoomChange(const RoomType &room);

private:
	ProfileSwitcher * profiles;
	ProfileEdit		* profileEditor;

	Interface::CheckButton btnHideMenu;
	Interface::Button btnRoomSwitch;
	Interface::Select selectProfile;

	Box getBorder();
	
	void updateAll(const RoomType &room);
	void updateRoomBtn(const RoomType &room);
	void updateProfileList();
	void updateLayout();
	
	void onRoomButtonClick();
	void onProfileMenuChange(int itemID, std::string itemName);
	void onProfileMenuClick(mouseButton button);
};


#endif
#include "glv_MenuBar.h"
#include "gld_types.h"

#define GLV_PROFILE_CAPTION_ADDNEW "Create User Profile"

#define GLV_MODE_CUSTOMIZE "Customization"
#define GLV_MODE_VIEWING   "Viewing Mode"

#define GLV_BTN_HIDE_WIDTH 25


MenuBar::MenuBar(ProfileSwitcher *pSwitcher, ProfileEdit *pEdit) {
	profiles = pSwitcher;
	profileEditor = pEdit;
};

bool MenuBar::isUnderCursor() {
	if (!btnHideMenu.checked)
		return btnHideMenu.isHovered();
	bool A = getBorder().contains(input.mouse);
	bool B = selectProfile.isOpened();
	return A || B;
};

void MenuBar::init(const RoomType &room) {
	btnHideMenu.caption = "";
	btnHideMenu.height = GLV_MENU_HEIGHT;
	btnHideMenu.width = GLV_BTN_HIDE_WIDTH;
	btnHideMenu.checked = true;

	btnRoomSwitch.caption = GLV_MODE_CUSTOMIZE;
	btnRoomSwitch.colorBorder = &clGray80;
	btnRoomSwitch.colorBorderHover = &clGray90;
	selectProfile.colorBorder = &clGray80;
	selectProfile.colorBorderHover = &clGray90;

	using namespace std::placeholders;
	selectProfile.callOnChange = std::bind(&MenuBar::onProfileMenuChange, this, _1, _2);
	selectProfile.callOnClick = std::bind(&MenuBar::onProfileMenuClick, this, _1);
	btnRoomSwitch.callOnClick = std::bind( &MenuBar::onRoomButtonClick, this );

	updateAll(room);
};

Box MenuBar::getBorder() {
	return Box(-1, canvas.bottom-GLV_MENU_HEIGHT, canvas.right+1, canvas.bottom+1);
};


////    UPDATES    ////

void MenuBar::updateAll(const RoomType &room) {
	updateRoomBtn(room);
	updateLayout();
	updateProfileList();
};

void MenuBar::updateLayout() {
	int y_menu = canvas.bottom - GLV_MENU_HEIGHT;
	btnHideMenu.setPosition(0, y_menu);
	int x = btnHideMenu.width + 5;
	btnRoomSwitch.setPosition(x, y_menu + 3);
	x += btnRoomSwitch.width + 5;
	selectProfile.setPosition(x, y_menu + 3);
};

void MenuBar::updateRoomBtn(const RoomType &room) {
	int oldWidth = btnRoomSwitch.width;
	if (room == rmVisual)
		btnRoomSwitch.caption = GLV_MODE_VIEWING;
	else if (room == rmConfig)
		btnRoomSwitch.caption = GLV_MODE_CUSTOMIZE;
	btnRoomSwitch.width = max(oldWidth, btnRoomSwitch.width);
};

void MenuBar::updateProfileList() {
	selectProfile.clearAllItems();
	selectProfile.addItem( GLV_PROFILE_CAPTION_ADDNEW );
	selectProfile.addItem( Interface::NULL_ITEM );
	for (unsigned i = 0; i < profiles->count(); i++)
		selectProfile.addItem( profiles->getName(i).c_str() );
	selectProfile.selectItem(profiles->getNameOfSelected());
	if (profiles->count() <= 1)
		selectProfile.selectItem(0);
};


////    EVENTS   ////

void MenuBar::onRoomButtonClick() {
	if (input.mousePressed(mbLeft)) {
		if (callRoomChange) 
			callRoomChange();
	}
};

void MenuBar::onProfileMenuClick(mouseButton button) {
	if (profiles->count() <= 1 && button == mbLeft)
		profileEditor->startAddingNew();
};

void MenuBar::onProfileMenuChange(int itemID, std::string itemName) {
	if (itemID < 2) 
		return;
	if (input.mousePressed(mbLeft) || itemID <= 2) {
		if (itemName == GLV_PROFILE_CAPTION_ADDNEW) {
			profileEditor->startAddingNew();
		}
		else {
			if (2 <= itemID) {
				profiles->select(itemID-2);
				updateProfileList();
				if (callProfileChange)
					callProfileChange();
			}
		}
	} 
	else if (input.mousePressed(mbRight) && 2 < itemID)
		profileEditor->startEditing(itemID-2);
};

void MenuBar::onRender() {
	if (btnHideMenu.checked) {
		drawBox(&getBorder(), clGray60);
		btnRoomSwitch.onRender();
		selectProfile.onRender();
	}
	btnHideMenu.onRender();
};

void MenuBar::onMouseDown(mouseButton button) {

	// Check if menu is not hidden
	btnHideMenu.onMouseDown(button);
	if (!btnHideMenu.checked)
		return;

	// Profile Selection Box
	selectProfile.onMouseDown(button);

	// Visual/Configuration Switch
	btnRoomSwitch.onMouseDown(button);

};

void MenuBar::onCanvasSizeChange(int width, int height) {
	updateLayout();
};


void MenuBar::onProfileEditClose(const RoomType &room) {
	updateProfileList();
	updateRoomBtn(room);
};

void MenuBar::onRoomChange(const RoomType &room) {
	updateRoomBtn(room);
};

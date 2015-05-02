#include "glv_MenuBar.h"
#include "gld_types.h"

#define MENU_PROFILE_RENAME "Rename"
#define MENU_PROFILE_REMOVE "Delete"


#define PROFILE_CREATE "Create User Profile"
#define PROFILE_CREATE "Create User Profile"

#define MODE_CUSTOMIZE "Customization"
#define MODE_VIEWING   "Viewing Mode"

#define MENU_HEIGHT 30
#define BTN_HIDE_WIDTH 25


MenuBar::MenuBar(ProfileSwitcher *pSwitcher, ProfileEdit *pEdit) {
	profiles = pSwitcher;
	profileEditor = pEdit;
};

bool MenuBar::isUnderCursor() {
	if (!btnHideMenu.checked)
		return btnHideMenu.isHovered();
	bool A = getBorder().contains(input.mouse);
	bool B = menuProfileSelect.visible;
	bool C = menuProfileEdit.visible;
	//bool D = selectProfile.isActive();
	return A || B || C ;//|| D;
};

void MenuBar::init() {
	btnHideMenu.caption = " ";
	btnHideMenu.height = MENU_HEIGHT;
	btnHideMenu.width = BTN_HIDE_WIDTH;
	btnHideMenu.checked = true;

	btnProfileSelect.colorBorder = &clGray80;
	btnProfileSelect.colorBorderHover = &clGray90;
	btnRoomSwitch.colorBorder = &clGray80;
	btnRoomSwitch.colorBorderHover = &clGray90;
	//selectProfile.colorBorder = &clGray80;
	//selectProfile.colorBorderHover = &clGray90;

	menuProfileEdit.addItem(MENU_PROFILE_RENAME);
	menuProfileEdit.addItem(MENU_PROFILE_REMOVE);

	using namespace std::placeholders;
	menuProfileSelect.callOnItemClick = std::bind(&MenuBar::onMenuProfilesClick, this, _1, _2);
	menuProfileEdit.callOnItemClick = std::bind(&MenuBar::onMenuProfileEditClick, this, _1, _2);
	//selectProfile.callOnChange = std::bind(&MenuBar::onMenuProfilesClick, this, _1, _2);
	btnProfileSelect.callOnClick = std::bind(&MenuBar::onProfileButtonClick, this);
	btnRoomSwitch.callOnClick = std::bind( &MenuBar::onRoomButtonClick, this );
};

void MenuBar::reloadProfileList() {
	menuProfileSelect.clearAllItems();
	menuProfileSelect.addItem( PROFILE_CAPTION_ADDNEW );
	menuProfileSelect.addItem( Interface::NULL_ITEM );
	for (unsigned i = 0; i < profiles->count(); i++)
		menuProfileSelect.addItem( profiles->getName(i).c_str() );

	//selectProfile.clearAllItems();
	//selectProfile.addItem( PROFILE_CAPTION_ADDNEW );
	//selectProfile.addItem( NULL_ITEM );
	//for (unsigned i = 0; i < profiles->count(); i++)
	//	selectProfile.addItem( profiles->getName(i).c_str() );
	//selectProfile.select(profiles->getNameOfSelected());
};

Box MenuBar::getBorder() {
	return Box(-1, canvas.bottom-MENU_HEIGHT, canvas.right+1, canvas.bottom+1);
};

void MenuBar::update(const RoomType &room) {
	int oldWidth = btnRoomSwitch.width;
	if (room == rmVisual)
		btnRoomSwitch.caption = MODE_VIEWING;
	else if (room == rmConfig)
		btnRoomSwitch.caption = MODE_CUSTOMIZE;
	update();
	reloadProfileList();
};

void MenuBar::update() {
	if (profiles->count() > 1)
		btnProfileSelect.caption = profiles->getNameOfSelected();
	else
		btnProfileSelect.caption = PROFILE_CREATE;
	
	int y_menu = canvas.bottom - MENU_HEIGHT;
	btnHideMenu.setPosition(0, y_menu);
	int x = btnHideMenu.width + 5;
	btnRoomSwitch.setPosition(x, y_menu + 3);
	x += btnRoomSwitch.width + 5;
	btnProfileSelect.setPosition(x, y_menu + 3);
	x += btnProfileSelect.width + 5;
	//selectProfile.setPosition(x, y_menu + 3);
};

void MenuBar::onRoomButtonClick() {
	if (input.mousePressed(mbLeft)) {
		if (funcRoomChange) 
			funcRoomChange();
	}
};


void MenuBar::onMenuProfilesClick(int itemID, std::string itemName) {
	if (itemName == PROFILE_CAPTION_ADDNEW && input.mousePressed(mbLeft)) {
		profileEditor->startAddingNew();
		menuProfileSelect.hide();
	} 
	else {
		if (input.mousePressed(mbRight) && 2<itemID) {
			menuProfileEdit.show(-1,-1);
			markedProfile = itemID - 2;
		}
		else if (input.mousePressed(mbLeft) && 2<=itemID) {
			profiles->select(itemID-2);
			update();
			if (funcProfileChange)
				funcProfileChange();
			menuProfileSelect.hide();
		}
	}
};


void MenuBar::onProfileButtonClick() {
	if (input.mousePressed(mbLeft)) {
		if (profiles->count() > 1)
			menuProfileSelect.show(btnProfileSelect.position.x, btnProfileSelect.position.y);
		else
			profileEditor->startAddingNew();
	}
};


void MenuBar::onMenuProfileEditClick(int itemID, std::string itemName) {
	menuProfileEdit.hide();
	menuProfileSelect.hide();
	if (itemName == MENU_PROFILE_RENAME) {
		profileEditor->startRenaming(markedProfile);
	}
	else 
	if (itemName == MENU_PROFILE_REMOVE) {
		profiles->remove(markedProfile);
		update();
	}
	markedProfile = -1;
};

void MenuBar::onRender() {
	if (btnHideMenu.checked) {
		drawBox(&getBorder(), clGray60);
		btnRoomSwitch.onRender();
		btnProfileSelect.onRender();
		if (profiles->count() > 1) {
			menuProfileSelect.onRender(!menuProfileEdit.visible);
			menuProfileEdit.onRender();
		}
		//selectProfile.draw();
	}
	btnHideMenu.onRender();
};

void MenuBar::onMouseDown(mouseButton button) {

	// Check if menu is not hidden
	btnHideMenu.onMouseDown(button);
	if (!btnHideMenu.checked)
		return;

	//selectProfile.onMouseDown(button);

	// Visual/Configuration Switch
	btnRoomSwitch.onMouseDown(button);

	// Profile Selection
	if (profiles->count() > 1) {
		
		if (menuProfileEdit.visible)
			menuProfileEdit.onMouseDown(button);
		else
		if (menuProfileSelect.visible)
			menuProfileSelect.onMouseDown(button);			
		else 
			btnProfileSelect.onMouseDown(button);
			
	} else
		btnProfileSelect.onMouseDown(button);
};

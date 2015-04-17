#include "glv_MenuBar.h"
#include "gld_types.h"

#define MENU_PROFILE_RENAME "Rename"
#define MENU_PROFILE_REMOVE "Delete"

#define MENU_HEIGHT 30


MenuBar::MenuBar(ProfileSwitcher *pSwitcher, ProfileEdit *pEdit) {
	profiles = pSwitcher;
	profileEditor = pEdit;
};

bool MenuBar::isUnderCursor() {
	bool A = getBorder().contains(input.mouse);
	bool B = menuProfileSelect.isActive();
	bool C = menuProfileEdit.isActive();
	return A || B || C;
};

void MenuBar::init() {
	btnProfileSelect.color.border = &clGray80;
	btnProfileSelect.color.hoverBorder = &clGray90;
	btnRoomSwitch.color.border = &clGray80;
	btnRoomSwitch.color.hoverBorder = &clGray90;

	reloadProfileList();

	menuProfileEdit.addItem(MENU_PROFILE_RENAME);
	menuProfileEdit.addItem(MENU_PROFILE_REMOVE);

	using namespace std::placeholders;
	menuProfileSelect.OnItemClick = std::bind(&MenuBar::onMenuProfilesClick, this, _1, _2);
	menuProfileEdit.OnItemClick = std::bind(&MenuBar::onMenuProfileEditClick, this, _1, _2);
	btnProfileSelect.OnClick = std::bind(&MenuBar::onProfileButtonClick, this);
	btnRoomSwitch.OnClick = std::bind( &MenuBar::onRoomButtonClick, this );
};

void MenuBar::reloadProfileList() {
	menuProfileSelect.clearAllItems();
	menuProfileSelect.addItem( PROFILE_CAPTION_ADDNEW );
	menuProfileSelect.addItem( NULL_ITEM );
	for (unsigned i = 0; i < profiles->count(); i++)
		menuProfileSelect.addItem( profiles->getName(i).c_str() );
};

Box MenuBar::getBorder() {
	return Box(-1, canvas.bottom-MENU_HEIGHT, canvas.right+1, canvas.bottom+1);
};

void MenuBar::update(const RoomType &room) {
	if (room == rmVisual)
		btnRoomSwitch.caption = "Customization";
	else if (room == rmConfig)
		btnRoomSwitch.caption = "Viewing Mode";
	update();
	reloadProfileList();
};

void MenuBar::update() {
	if (profiles->count() > 1)
		btnProfileSelect.caption = profiles->getNameOfSelected();
	else
		btnProfileSelect.caption = "Create User Profile";

	int ys = canvas.bottom - MENU_HEIGHT;
	btnRoomSwitch.setPosition(5, ys + 3);
	btnProfileSelect.setPosition(btnRoomSwitch.width + 10, ys + 3);
};

void MenuBar::onRoomButtonClick() {
	if (input.mousePressed(mbLeft)) {
		if (funcRoomChange) 
			funcRoomChange();
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

void MenuBar::onMenuProfilesClick(int itemID, std::string itemName) {
	menuProfileSelect.hide();
	if (itemName == PROFILE_CAPTION_ADDNEW)
		profileEditor->startAddingNew();
	else 
	if ( 1 < itemID ) {
		if (input.mousePressed(mbRight)) {
			menuProfileEdit.show(-1,-1);
			markedProfile = itemID - 2;
		}
		else {
			profiles->select(itemID-2);
			update();
			if (funcProfileChange)
				funcProfileChange();
		}
	} 
};

void MenuBar::onMenuProfileEditClick(int itemID, std::string itemName) {
	menuProfileEdit.hide();
	menuProfileSelect.hide();
	if (itemName == MENU_PROFILE_RENAME) {
		profileEditor->startRenaming(markedProfile);
	}
	else 
	if (itemName == MENU_PROFILE_REMOVE)
		profiles->remove(markedProfile);
	markedProfile = -1;
};

void MenuBar::onRender() {
	drawBox(&getBorder(), clGray60);
	btnRoomSwitch.draw();
	btnProfileSelect.draw();
	if (profiles->count() > 1) {
		menuProfileSelect.draw(!menuProfileEdit.isActive());
		menuProfileEdit.draw();
	}
};

void MenuBar::onMouseDown(mouseButton button) {
	//   Visual/Configuration Switch
	btnRoomSwitch.onMouseDown(button);

	//   Profile Selection
	if (profiles->count() > 1) {
		if (menuProfileEdit.isActive())
			menuProfileEdit.onMouseDown(button);
		else
		if (menuProfileSelect.isActive())
			menuProfileSelect.onMouseDown(button);			
		else 
			btnProfileSelect.onMouseDown(button);
	}
};

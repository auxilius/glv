#include "gld_engine.h"

#include <gl/glew.h>
#include <gl/GLU.h>

#pragma region ProfileEdit Class

class ProfileEdit {
private:
	int editedProfile;
	std::string text;
	Button finish, cancel;
	Box editBorder;
public:
	bool isActive;

	bool changedProfiles;
	ProfileEdit();
	void recalculatePositions();
	void render();
	void mouseDown(mouseButton button);
	void keyDown(char key);

	void startRenaming(int profile);
	void startAdding();
};

ProfileEdit::ProfileEdit() {
	editedProfile = -1;
	isActive = false;
	changedProfiles = false;
	finish.caption = "OK";
	cancel.caption = "Cancel";
	editBorder.setSize(250, fontMain->getSize()+10);
	recalculatePositions();
};
void ProfileEdit::startRenaming(int profile) {
	recalculatePositions();
	editedProfile = profile;	
	isActive = true;
	text = profileSwitcher.getProfileName(profile);
};
void ProfileEdit::startAdding() {
	recalculatePositions();
	isActive = true;
	editedProfile = -1;
	text = "";
};
void ProfileEdit::recalculatePositions() {
	int y = canvas.height / 2 - 25;
	int x = (canvas.width - 250) / 2;
	editBorder.moveTo(x, y);
	int buttonsWidth = finish.width + cancel.width + 10;
	x = (canvas.width - buttonsWidth) / 2;
	y += editBorder.height + 10;
	finish.setPosition(x, y);
	x += finish.width + 10;
	cancel.setPosition(x, y);
};
void ProfileEdit::render() {
	glColor(150, 150, 150);
	std::string caption = (editedProfile==-1) ? "Name of new profile" : (std::string)"Rename: "+profileSwitcher.getProfileName(editedProfile);
	fontLarge->textOut(canvas.width/2, editBorder.top - fontLarge->getSize() - 10, caption, faCenter);
	drawRect(GL_LINE_LOOP, editBorder);
	glColor(200, 200, 200);
	fontMain->textOut(editBorder.left + 10, editBorder.top + 5, text);
	finish.draw();
	cancel.draw();
};
void ProfileEdit::mouseDown(mouseButton button) {
	if (finish.isClicked())
		keyDown(KEY_ENTER);
	if (cancel.isClicked())
		keyDown(KEY_ESCAPE);
};
void ProfileEdit::keyDown(char key) {
	if (key == KEY_BACKSPACE) { // BACKSPACE
		if (text.length() > 0)
			text.resize(text.length() - 1);
	}
	else if (key == KEY_ENTER) { // ENTER
		if (editedProfile == -1)
			profileSwitcher.addProfile(text);
		else
			profileSwitcher.renameProfile(editedProfile, text);
		isActive = false;
		changedProfiles = true;
	}
	else if (key == KEY_ESCAPE) { // ESCAPE
		isActive = false;
	}
	else if ( (key>='a' && key<='z') || (key>='A' && key<='Z') || key==KEY_SPACE ) {
		if (text.length() < 23)
			text += key;
	}
};
#pragma endregion

ProfileEdit * profileEditScreen;


void gldEngine::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	profileSwitcher.init();

	menu_profileEdit.addItem("Rename");
	menu_profileEdit.addItem("Delete");

	menu_profileSelectLoad();
	
	button_profileSelect.setCaption("Set profile");

	panelCalculate();

	reloadProjection();
	configManager.init();
	visualizer.init();
	
	visualizer.renderTrigger = &needToRender;
	
	configManager.load();
	visualizer.load();
	room = (configLoader.fieldsBlank()) ? rmConfig : rmVisual;

	profileEditScreen = new ProfileEdit();
};

void gldEngine::menu_profileSelectLoad() {
	menu_profileSelect.addItem( PROFILE_CAPTION_ADDNEW );
	menu_profileSelect.addItem( POPUPMENU_NULL );
	for (int i = 0; i < (int)profileSwitcher.count(); i++)
		menu_profileSelect.addItem( profileSwitcher.getProfileName(i) );
};

void gldEngine::panelCalculate() {
	if (room == rmVisual)
		button_modeSwitch.caption = "Customization";
	else if (room == rmConfig)
		button_modeSwitch.caption = "Visualization";
	
	if (profileSwitcher.count() > 1)
		button_profileSelect.caption = profileSwitcher.getProfileName();
	else
		button_profileSelect.caption = "Create User Profile";

	int ys = canvas.bottom - MENU_HEIGHT;
	button_modeSwitch.setPosition(5, ys + 3);
	button_profileSelect.setPosition(button_modeSwitch.width + 10, ys + 3);
};

void gldEngine::panelRender() {
	Box menuBar;
	menuBar.set(0, canvas.bottom-MENU_HEIGHT, canvas.right, canvas.bottom);
	glColor3f(0.3f, 0.3f, 0.3f);
	drawRect(GL_QUADS, menuBar);
	/* 
		Color Scale
	if (room == rmVisual && form.width > button_modeSwitch.width) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f);

		fontSmall->textOut(menuBar.right - 115, menuBar.top + 5, "Value scale 0-1");
		
		glBegin(GL_LINES);
		for (float i = 0.0f; i <= 1.0f; i += 0.01f) {
			valToColor_Rainbow(i);
			glVertex2f((GLfloat)menuBar.right - 110.0f + i * 100.0f, (GLfloat)menuBar.bottom - 10.0f);
			glVertex2f((GLfloat)menuBar.right - 110.0f + i * 100.0f, (GLfloat)menuBar.bottom - 5.0f);
		}
		glEnd();
	}
	*/
	button_modeSwitch.draw();

	button_profileSelect.draw();
	if (profileSwitcher.count() > 1) {
		menu_profileSelect.draw();
		menu_profileEdit.draw();
	}
};

void gldEngine::render() {
	reloadProjection();
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	if (profileEditScreen->isActive) {
		profileEditScreen->render();
		return;
	}
	if (profileEditScreen->changedProfiles) {
		profileEditScreen->changedProfiles = false;
		menu_profileSelect.clearAllItems();
		menu_profileSelectLoad();
		visualizer.load();
		configManager.load();
		room = (configLoader.fieldsBlank()) ? rmConfig : rmVisual;
		panelCalculate();
	}

	if (room == rmConfig)
		configManager.render();
	else if (room == rmVisual)
		visualizer.render();

	panelRender();
};

void gldEngine::onCanvasSizeChange(int width, int height) {
	panelCalculate();
	if (profileEditScreen != NULL)
		profileEditScreen->recalculatePositions();
};

bool gldEngine::panelMouseDown(mouseButton button) {

	//   Visual/Configuration Switch
	if (button_modeSwitch.isClicked()) {
		if (room == rmConfig)  {
			configManager.save();
			if (!configLoader.fieldsBlank()) {
				room = rmVisual;
				visualizer.load();
			} else {
				MessageBox(0, ERRORTEXT_CANNOT_SWITCH_MODE, ERRORTEXT_HEADER, MB_OK | MB_ICONINFORMATION);
			}
		}
		else if (room == rmVisual) {
			visualizer.save();
			room = rmConfig;
		}
		panelCalculate();
		return true;
	}

	//   Profile Selection
	if (profileSwitcher.count() > 1) {
		
		static int markedProfile = -1;

		if (button == mbRight) {
			int hoveredItem = menu_profileSelect.hoveredItemNumber();
			if (hoveredItem > 2) {
				menu_profileEdit.show(-1,-1,true);
				markedProfile = hoveredItem - 2;
				return true;
			}
		}
		
		if (button == mbLeft) {
			
			//   Profile Edit menu
			int selectedItem = menu_profileEdit.selectedItemNumber();
			if (selectedItem != -1) {
				if (selectedItem == 0) {
					profileEditScreen->startRenaming(markedProfile);
				}
				else if (selectedItem == 1) {
					profileSwitcher.deleteProfile(markedProfile);
					profileEditScreen->changedProfiles = true;
				}
				menu_profileEdit.hide();
				menu_profileSelect.hide();
				markedProfile = -1;
				return true;
			}

			//   Profile Select menu
			selectedItem = menu_profileSelect.selectedItemNumber();
			if (selectedItem != -1) {
				menu_profileSelect.hide();
				if (selectedItem == 0) {
					profileEditScreen->startAdding();
					return true;
				}
				int profile_selected = selectedItem - 2;
				profileSwitcher.selectProfile(profile_selected);
				configManager.load();
				visualizer.load();
				room = (configLoader.fieldsBlank()) ? rmConfig : rmVisual;
			
				panelCalculate();
				return true;
			} 
			else if (button_profileSelect.isClicked()) {

				menu_profileSelect.show(button_profileSelect.position.x, button_profileSelect.position.y, true);
				return true;
			}
		}
	}
	else if (button_profileSelect.isClicked()) {
		profileEditScreen->startAdding();
		return true;
	}

	return false;
};

void gldEngine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);

	if (profileEditScreen->isActive) {
		profileEditScreen->mouseDown(button);
		return;
	}
	
	if (!panelMouseDown(button)) {
		if (room == rmConfig)
			configManager.mouseDown(button);
		else if (room == rmVisual)
			visualizer.mouseDown(button);
	}
};

void gldEngine::onMouseUp(mouseButton button) {
	input.setMouseButtonUp(button);
	if (room == rmConfig)
		configManager.mouseUp(button);
	else if (room == rmVisual)
		visualizer.mouseUp(button);
};

void gldEngine::onMouseMove(int x, int y) {
	if (room == rmVisual)
		visualizer.mouseMove(x, y);
};

void gldEngine::onMouseWheel(signed short direction) {
	if (room == rmVisual)
		visualizer.mouseWheel(direction);
};

void gldEngine::onKeyDown(char key) {
	if (profileEditScreen->isActive) {
		profileEditScreen->keyDown(key);
		return;
	}
};
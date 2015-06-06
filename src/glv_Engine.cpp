#include "glv_Engine.h"

#include <gl/glew.h>
#include <gl/GLU.h>
#include "glv_Shaders.h"

using namespace glv;

glv_Engine::glv_Engine() {
	shader = new Shaders();
	
	fieldManager = new FieldManager();
	
	profiles = new ProfileSwitcher(fieldManager);
	profileEditCtrl = new ProfileEdit(profiles);
	
	viewModeCtrl = new ViewModeControl(fieldManager);
	configModeCtrl = new ConfigModeControl(fieldManager);
	
	menuBar = new MenuBar(profiles, profileEditCtrl);
	menuBar->callRoomChange = std::bind(&glv_Engine::onRoomChange, this);
	menuBar->callProfileChange = std::bind(&glv_Engine::onProfileChange, this);

	actualRoom = rmConfig;
};

void glv_Engine::init() {
	shader->init();

	// OpenGL init
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	reloadProjection();

	profiles->init();
	configModeCtrl->init();
	viewModeCtrl->init();
	viewModeCtrl->renderTrigger = &needToRender;
	
	viewModeCtrl->loadParams();
	actualRoom = (fieldManager->isBlank()) ? rmConfig : rmVisual;

	menuBar->init(actualRoom);
};

void glv_Engine::render() {
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glActiveTexture(GL_TEXTURE0);
	reloadProjection();
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	static bool editedProfiles = false;
	if (profileEditCtrl->isActive()) {
		editedProfiles = true;
		profileEditCtrl->render();
		return;
	}
	if (editedProfiles) {
		editedProfiles = false;
		viewModeCtrl->loadParams();
		actualRoom = (fieldManager->isBlank()) ? rmConfig : rmVisual;
		menuBar->onProfileEditClose(actualRoom);
	}

	if (actualRoom == rmConfig)
		configModeCtrl->render();
	else 
	if (actualRoom == rmVisual)
		viewModeCtrl->render();

	menuBar->onRender();
};



void glv_Engine::onCanvasSizeChange(int width, int height) {
	menuBar->onCanvasSizeChange(width, height);
	if (profileEditCtrl != NULL)
		profileEditCtrl->recalculatePositions();
};

void glv_Engine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);

	if (profileEditCtrl->isActive()) {
		profileEditCtrl->mouseDown(button);
		return;
	}

	if (menuBar->isUnderCursor())
		menuBar->onMouseDown(button);
	else	
	if (actualRoom == rmConfig)
		configModeCtrl->mouseDown(button);
	else 
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseDown(button);
};

void glv_Engine::onMouseUp(mouseButton button) {
	input.setMouseButtonUp(button);
	if (actualRoom == rmConfig)
		configModeCtrl->mouseUp(button);
	else 
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseUp(button);
};

void glv_Engine::onMouseMove(int x, int y) {
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseMove(x, y);
};

void glv_Engine::onMouseWheel(signed short direction) {
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseWheel(direction);
};

void glv_Engine::onKeyDown(char key) {
	if (profileEditCtrl->isActive())
		profileEditCtrl->keyDown(key);
};

void glv_Engine::onRoomChange(void) {
	if (actualRoom == rmConfig)  {
		if (!fieldManager->isBlank()) {
			actualRoom = rmVisual;
			viewModeCtrl->loadParams();
		} else 
			MessageBox(0, "Create at least one field and select its type to proceed to visualization mode.", GLV_ERRORTEXT_HEADER, MB_OK | MB_ICONINFORMATION);
	}
	else if (actualRoom == rmVisual) {
		viewModeCtrl->saveParams();
		actualRoom = rmConfig;
	}
	menuBar->onRoomChange(actualRoom);
};

void glv_Engine::onProfileChange(void) {
	viewModeCtrl->loadParams();
	if (actualRoom == rmVisual && fieldManager->isBlank())
		actualRoom = rmConfig;
};



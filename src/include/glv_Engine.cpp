#include "glv_Engine.h"

#include <gl/glew.h>
#include <gl/GLU.h>
#include "glv_Shaders.h"

Engine::Engine() {
	shader = new Shaders();
	
	fieldManager = new FieldManager();
	
	profiles = new ProfileSwitcher(fieldManager);
	profileEditCtrl = new ProfileEdit(profiles);
	
	viewModeCtrl = new ViewModeControl(fieldManager);
	configModeCtrl = new ConfigModeControl(fieldManager);
	
	menuBar = new MenuBar(profiles, profileEditCtrl);
	menuBar->funcRoomChange = std::bind(&Engine::onRoomChange, this);
	menuBar->funcProfileChange = std::bind(&Engine::onProfileChange, this);

	actualRoom = rmConfig;
};

void Engine::init() {
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

	menuBar->init();
	menuBar->update(actualRoom);
};

void Engine::render() {
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
		menuBar->reloadProfileList();
		viewModeCtrl->loadParams();
		actualRoom = (fieldManager->isBlank()) ? rmConfig : rmVisual;
		menuBar->update(actualRoom);

	}

	if (actualRoom == rmConfig)
		configModeCtrl->render();
	else 
	if (actualRoom == rmVisual)
		viewModeCtrl->render();

	menuBar->onRender();
};



void Engine::onCanvasSizeChange(int width, int height) {
	menuBar->update(actualRoom);
	if (profileEditCtrl != NULL)
		profileEditCtrl->recalculatePositions();
};

void Engine::onMouseDown(mouseButton button) {
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

void Engine::onMouseUp(mouseButton button) {
	input.setMouseButtonUp(button);
	if (actualRoom == rmConfig)
		configModeCtrl->mouseUp(button);
	else 
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseUp(button);
};

void Engine::onMouseMove(int x, int y) {
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseMove(x, y);
};

void Engine::onMouseWheel(signed short direction) {
	if (actualRoom == rmVisual)
		viewModeCtrl->mouseWheel(direction);
};

void Engine::onKeyDown(char key) {
	if (profileEditCtrl->isActive())
		profileEditCtrl->keyDown(key);
};

void Engine::onRoomChange(void) {
	if (actualRoom == rmConfig)  {
		if (!fieldManager->isBlank()) {
			actualRoom = rmVisual;
			viewModeCtrl->loadParams();
		} else 
			MessageBox(0, ERRORTEXT_CANNOT_SWITCH_MODE, ERRORTEXT_HEADER, MB_OK | MB_ICONINFORMATION);
	}
	else if (actualRoom == rmVisual) {
		viewModeCtrl->saveParams();
		actualRoom = rmConfig;
	}
	menuBar->update(actualRoom);
};

void Engine::onProfileChange(void) {
	viewModeCtrl->loadParams();
	if (actualRoom == rmVisual && fieldManager->isBlank())
		actualRoom = rmConfig;
};



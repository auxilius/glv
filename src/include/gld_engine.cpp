#include <gl/glew.h>
#include <gl/GLU.h>
#include "gld_engine.h"

unsigned renders = 0;

void gldEngine::panelCalculate() {
	if (room == rmVisual)
		button_modeSwitch.caption = "Customization";
	else if (room == rmConfig)
		button_modeSwitch.caption = "Visualization";
	button_profileSelect.caption = configLoader.getProfileName();
	

	int ys = canvas.bottom - MENU_HEIGHT;
	button_modeSwitch.setPosition(5, ys + 3);
	button_profileSelect.setPosition(button_modeSwitch.width + 10, ys + 3);
}
void gldEngine::panelMouseDown() {

	if (button_modeSwitch.isClicked()) {
		if (room == rmConfig)  {
			configManager.save();
			if (configLoader.valid()) {
				room = rmVisual;
				visualizer.load();
			} else
				return;
		}
		else if (room == rmVisual) {
			visualizer.save();
			room = rmConfig;
		}
		panelCalculate();
	}

	if (configLoader.profiles.size() > 1) {
		int select = menu_profileSelect.selectedItemNumber();
		if (select != -1) {
			menu_profileSelect.hide();
			configLoader.selectProfile(select);
			configManager.load();
			visualizer.load();
			panelCalculate();
		} else
		if (button_profileSelect.isClicked())
			menu_profileSelect.show(button_profileSelect.position.x, button_profileSelect.position.y, true);
	}
}
void gldEngine::panelRender() {
	Box menuBar;
	menuBar.set(0, canvas.bottom-MENU_HEIGHT, canvas.right, canvas.bottom);
	glColor3f(0.3f, 0.3f, 0.3f);
	drawRect(GL_QUADS, menuBar);
	/*
	glColor3f(0.4f, 0.4f, 0.4f);
	drawRect(GL_QUADS, switchButton);
	if (room == rmConfig && !configLoader.valid())
		glColor(80,10,10);
	else if (switchButton.contains(input.mouse)) 
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	else 
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);	
	drawSwitchIcon(switchButton.left + switchButton.height / 2, switchButton.center.y, switchButton.height);
	
	setFontSize(8);
	textPrint(switchButton.left + switchButton.height + 5, switchButton.top + 3, text);
	*/
	
	if (room == rmVisual && form.width > button_modeSwitch.width) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
		setFontSize(6);
		textPrint(menuBar.right - 115, menuBar.top + 5, "Value scale 0-1");
		glBegin(GL_LINES);
		for (float i = 0.0f; i <= 1.0f; i += 0.01f) {
			setColorByValue(i);
			glVertex2f(menuBar.right - 110 + i * 100, menuBar.bottom - 10);
			glVertex2f(menuBar.right - 110 + i * 100, menuBar.bottom - 5);
		}
		glEnd();
	}

	button_modeSwitch.draw();
	if (configLoader.profiles.size() > 1) {
		button_profileSelect.draw();
		menu_profileSelect.draw();
	}
};

void gldEngine::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	configLoader.init();
	for (int i = 0; i < configLoader.profiles.size(); i++)
		menu_profileSelect.addItem(configLoader.profiles[i].c_str());

	button_modeSwitch.setFont(8);
	button_profileSelect.setFont(8);
	button_profileSelect.setCaption("Set profile");
	panelCalculate();

	reloadProjection();
	configManager.init();
	visualizer.init();
	
	visualizer.renderTrigger = &needToRender;
	
	configManager.load();
	if (configLoader.valid()) {
		visualizer.load();
		room = rmVisual;
	} else
		room = rmConfig;
};

void gldEngine::render() {
	reloadProjection();
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (room == rmConfig)
		configManager.render();
	else if (room == rmVisual)
		visualizer.render();
	panelRender();
	renders++;
};

void gldEngine::onCanvasSizeChange(int width, int height) {
	panelCalculate();
};
void gldEngine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);
	if (room == rmConfig)
		configManager.mouseDown(button);
	else if (room == rmVisual)
		visualizer.mouseDown(button);
	panelMouseDown();
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

void gldEngine::searchForConfigFiles() {
	
};
#include <gl/glew.h>
#include <gl/GLU.h>
#include "gld_engine.h"

unsigned renders = 0;

void gldEngine::renderMenu() {
	Box menuBar;
	menuBar.set(0, canvas.bottom-MENU_HEIGHT, canvas.right, canvas.bottom);
	glColor3f(0.3f, 0.3f, 0.3f);
	drawRect(GL_QUADS, menuBar);

	glColor3f(0.4f, 0.4f, 0.4f);
	drawRect(GL_QUADS, switchButton);
	if (room == rmConfig && !configuration.valid())
		glColor(80,10,10);
	else if (switchButton.contains(input.mouse)) 
		glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	else 
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);	
	drawSwitchIcon(switchButton.left + switchButton.height / 2, switchButton.center.y, switchButton.height);
	std::string text = "";
	if (room == rmVisual)
		text = "Configure layout";
	if (room == rmConfig)
		text = "Start Visualization";
	setFontSize(8);
	textPrint(switchButton.left + switchButton.height + 5, switchButton.top + 3, text);

	if (room == rmVisual && form.width > switchButton.width) {
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
};
void gldEngine::processMenuClick(int x, int y, mouseButton button) {
	if (button != mbLeft || !switchButton.contains(input.mouse))
		return;
	if (room == rmConfig)  {
		configManager.save();
		if (configuration.valid()) {
			room = rmVisual;
			visualizer.load();
		} else
			return;
	}
	else if (room == rmVisual) {
		visualizer.save();
		room = rmConfig;
	}
	
};

void gldEngine::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	switchButton.set(3, 3, 180, 27);
	reloadProjection();
	configManager.init();
	visualizer.init();
	
	visualizer.renderTrigger = &needToRender;
	configuration.load();
	configManager.load();
	if (configuration.valid()) {
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
	renderMenu();
	renders++;
};

void gldEngine::onCanvasSizeChange(int width, int height) {
	switchButton.moveTo(5, canvas.bottom-25);
};
void gldEngine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);
	if (room == rmConfig)
		configManager.mouseDown(button);
	else if (room == rmVisual)
		visualizer.mouseDown(button);
};
void gldEngine::onMouseUp(mouseButton button) {
	input.setMouseButtonUp(button);
	if (room == rmConfig)
		configManager.mouseUp(button);
	else if (room == rmVisual)
		visualizer.mouseUp(button);
	processMenuClick(input.mouse.x, input.mouse.y, button);
};
void gldEngine::onMouseMove(int x, int y) {
	if (room == rmVisual)
		visualizer.mouseMove(x, y);
};
void gldEngine::onMouseWheel(signed short direction) {
	if (room == rmVisual)
		visualizer.mouseWheel(direction);
};

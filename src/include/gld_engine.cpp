#include <gl/glew.h>
#include <gl/GLU.h>
#include "gld_engine.h"

unsigned renders = 0;

void gldEngine::renderMenu() {
	Box menuBar;
	menuBar.set(0, canvas.bottom-MENU_HEIGHT, canvas.right, canvas.bottom);
	glColor3f(0.3f, 0.3f, 0.3f);
	drawRect(GL_QUADS, menuBar);
	glColor3f(0.6f, 0.6f, 0.6f);
	glLineWidth(1.0f);
<<<<<<< HEAD
	//drawLine(0, canvas.bottom-30, canvas.right, canvas.bottom-30);

	glColor3f(0.4f, 0.4f, 0.4f);
	drawRect(GL_QUADS, switchButton);
=======
	drawLine(0, canvas.bottom-30, canvas.right, canvas.bottom-30);

>>>>>>> d897c36f0ef0ba704fa81affdc366c35607ab3d0
	if (configurator.numberOfField() == 0)
		glColor4f(1.0f, 0.1f, 0.1f, 0.3f);
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
	if (button != mbLeft) 
		return;

	if ((configurator.numberOfField() != 0)
		&& switchButton.contains(input.mouse)
		&& (button == mbLeft)) {
		if (room == rmConfig) {
			configurator.saveConfiguration();
			room = rmVisual;
			visualizer.loadConfiguration();
		}
		else if (room == rmVisual)
			room = rmConfig;
	}
};

void gldEngine::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

<<<<<<< HEAD
	switchButton.set(3, 3, 180, 27);
=======
	switchButton.set(5, 5, 180, 25);
>>>>>>> d897c36f0ef0ba704fa81affdc366c35607ab3d0
	reloadProjection();
	configurator.init();
	visualizer.init();
	
	visualizer.renderTrigger = &needToRender;
	configurator.loadConfiguration();
	if (configurator.numberOfField() == 0) room = rmConfig;
	else {
		visualizer.loadConfiguration();
		room = rmVisual;
	}
	//visualizer.addValues("number of renders: %u", &renders);
};

void gldEngine::render() {
	reloadProjection();
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (room == rmConfig)
		configurator.render();
	else if (room == rmVisual)
		visualizer.render();
	renderMenu();
	renders++;
};

void gldEngine::saveConfiguration() {
	configurator.saveConfiguration();
};

void gldEngine::onCanvasSizeChange(int width, int height) {
	switchButton.moveTo(5, canvas.bottom-25);
};
void gldEngine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);
	if (room == rmConfig)
		configurator.mouseDown(button);
	else if (room == rmVisual)
		visualizer.mouseDown(button);
};
void gldEngine::onMouseUp(mouseButton button) {
	input.setMouseButtonUp(button);
	if (room == rmConfig)
		configurator.mouseUp(button);
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

<<<<<<< HEAD
void gldEngine::addTexture(const char * caption, const GLuint texture) {
	visualizer.addTexture(caption, texture);
}
void gldEngine::addValues(const char * caption, const char * format, void * data[]) {
	visualizer.addValues(caption, format, data);
}
void gldEngine::addModel(const char * caption, const GLuint vbo, const unsigned count) {
	visualizer.addModel(caption, vbo, count);
}
void gldEngine::addModelData(const char * caption, float * data, float min, float max) {
=======
void gldEngine::addTexture(char * caption, const GLuint texture) {
	visualizer.addTexture(caption, texture);
}
void gldEngine::addValue(char * caption, void * data) {
	visualizer.addValues(caption, data);
}
void gldEngine::addValues(char * caption, void * data[]) {
	visualizer.addValues(caption, data);
}
void gldEngine::addModel(char * caption, const GLuint vbo, const unsigned count) {
	visualizer.addModel(caption, vbo, count);
}
void gldEngine::addModelData(char * caption, float * data, float min, float max) {
>>>>>>> d897c36f0ef0ba704fa81affdc366c35607ab3d0
	visualizer.addModelData(caption, data, min, max);
}
void gldEngine::addModelData(const GLuint vbo, float * data, float min, float max) {
	visualizer.addModelData(vbo, data, min, max);
}
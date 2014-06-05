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
	if (room == rmConfig && configManager.numberOfField() == 0)
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
	if (button != mbLeft) 
		return;
	if (room == rmConfig && (configManager.numberOfField() == 0)) 
		return;
	if (switchButton.contains(input.mouse) && (button == mbLeft)) {
		if (room == rmConfig) {
			configManager.save();
			room = rmVisual;
			visualizer.load();
		}
		else if (room == rmVisual) {
			visualizer.save();
			room = rmConfig;
		}
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
	loadConfig();
	configManager.load();
	if (configuration.field.size() == 0) 
		room = rmConfig;
	else {
		visualizer.load();
		room = rmVisual;
	}
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

void gldEngine::addTexture(const char * caption, const GLuint texture) {
	visualizer.addTexture(caption, texture);
}
void gldEngine::addValues(const char * caption, const char * format, void * data[]) {
	visualizer.addValues(caption, format, data);
}
void gldEngine::addModel(const char * caption, const unsigned count, const GLuint vertices, const GLuint indices) {
	visualizer.addModel(caption, count, vertices, indices);
}
void gldEngine::addModelData(const char * caption, float * data, float min, float max) {
	visualizer.addModelData(caption, data, min, max);
}
void gldEngine::addModelData(const GLuint vbo, float * data, float min, float max) {
	visualizer.addModelData(vbo, data, min, max);
}

void gldEngine::saveConfig() {
	if (room == rmVisual)
		visualizer.save();
	if (room == rmConfig)
		configManager.save();
	std::ofstream stream;
	stream.open(FILE_CONFIG);
	if (stream.fail())
		MessageBox(0, L"Saving configuration failed", L"Cannot create configuration file, unknown error.", MB_OK);
	form.save(stream);
	stream << std::endl << std::endl;
	configuration.save(stream);
	stream.close();
};
void gldEngine::loadConfig() {
	std::ifstream stream;
	stream.open(FILE_CONFIG);
	if (stream.fail())
		return;
	unsigned x;
	stream >> x >> x >> x >> x;
	configuration.load(stream);
	stream.close();
	
};
#include "gld_engine.h"

#include <gl/glew.h>
#include <gl/GLU.h>

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
bool gldEngine::panelMouseDown() {

	if (button_modeSwitch.isClicked()) {
		if (room == rmConfig)  {
			configManager.save();
			if (configLoader.valid()) {
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

	if (configLoader.profiles.size() > 1) {
		int select = menu_profileSelect.selectedItemNumber();
		if (select != -1) {
			menu_profileSelect.hide();
			configLoader.selectProfile(select);
			configManager.load();
			visualizer.load();
			panelCalculate();
			if (configLoader.valid()) {
				visualizer.load();
				room = rmVisual;
			} else
				room = rmConfig;
			return true;
		} 
		else if (button_profileSelect.isClicked()) {
			menu_profileSelect.show(button_profileSelect.position.x, button_profileSelect.position.y, true);
			return true;
		}
	}
	return false;
}
void gldEngine::panelRender() {
	Box menuBar;
	menuBar.set(0, canvas.bottom-MENU_HEIGHT, canvas.right, canvas.bottom);
	glColor3f(0.3f, 0.3f, 0.3f);
	drawRect(GL_QUADS, menuBar);
	/*
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
	for (int i = 0; i < (int)configLoader.profiles.size(); i++)
		menu_profileSelect.addItem(configLoader.profiles[i].c_str());

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


	/*
	glColor3f(0.1f, 0.2f, 0.5f);
	Box rect; 
	rect.moveTo(10,10);
	rect.setSize( (int)font1->getWidth("Hello World! asdfghjkl"), (int)font1->getHeight() );
	//drawRect(GL_QUADS, rect );
	
	glColor3f(1.0f, 0.5f, 0.1f);
	
	font1->textOut("Hello World! asdfghjkl", 10, 20);
	font2->textOut("Hello World! asdfghjkl", 10, 50);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(5.0f, 1.0f); glVertex2f(5.0f, 20.0f);
	glVertex2f(5.0f, 30.0f); glVertex2f(5.0f, 50.0f);
	glEnd();
	*/
};

void gldEngine::onCanvasSizeChange(int width, int height) {
	panelCalculate();
};
void gldEngine::onMouseDown(mouseButton button) {
	input.setMouseButtonDown(button);
	if (!panelMouseDown()) {
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
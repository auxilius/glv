#include "gld_config.h"




//MessageBox(0, L"And text here", L"MessageBox caption", MB_OK);

using namespace std;


ConfigField::ConfigField(Box cBorder) {
	border.set(cBorder);
	visualizationType = 0;
};
ConfigField::ConfigField(int x1, int y1, int x2, int y2) {
	border.set(x1, y1, x2, y2);
	visualizationType = 0;
};
void ConfigField::makeSquare() {
	int size = max(border.width, border.height);
	border.setSize(size, size);
};
void ConfigField::moveScaleHandle(int x, int y) {
	if (x < border.left + FIELD_MIN_SIZE)
		x = border.left + FIELD_MIN_SIZE;
	border.right = x;
	if (y < border.top + FIELD_MIN_SIZE)
		y = border.top + FIELD_MIN_SIZE;
	border.bottom = y;
};
void ConfigField::moveScaleHandle(Point point) {
	moveScaleHandle(point.x, point.y);
};
void ConfigField::drawScaleHandle() {
	if (pointDistance(input.mouse, point(border.right, border.bottom)) < FIELD_SCALEHANDLE_SIZE) {
		glColor3f(0.8f, 0.8f, 0.8f);
		glEnable(GL_POINT_SMOOTH);
		glPointSize((GLfloat)FIELD_SCALEHANDLE_SIZE * 2);
		glBegin(GL_POINTS); glVertex2f((GLfloat)border.right, (GLfloat)border.bottom); glEnd();
	}
};
void ConfigField::drawText() {
	glColor4f(0.95f, 0.95f, 0.95f, 0.95f);
	double scale = (double)(border.width) / 20.0;
	if (scale < 8)  {
		setFontSize(20);
		std::string inicial = "";
		switch (visualizationType) {
		case FIELD_TYPE_NONE:	inicial = "X"; break;
		case FIELD_TYPE_TEXTURE:inicial = "T"; break;
		case FIELD_TYPE_MODEL:	inicial = "M"; break;
		case FIELD_TYPE_VALUE:	inicial = "V"; break;
		}
		textPrintCentered(border.center.x, border.center.y, inicial);
	}
	else {
		if (scale > 30) scale = 30;
		setFontSize(scale);
		textPrintCentered(border.center.x, border.center.y, visualizationTypeCaption[visualizationType]);
	}
};
void ConfigField::draw(bool selected) {
	if (visualizationType == FIELD_TYPE_NONE)
		glColor(145, 139, 141);
		//glColor3f(0.3f, 0.3f, 0.3f);
	else if (visualizationType == FIELD_TYPE_MODEL)
		glColor(61, 70, 53);
		//glColor3f(0.24313f, 0.21960f, 0.11765f);
	else if (visualizationType == FIELD_TYPE_TEXTURE)
		glColor(132, 146, 110);
		//glColor3f(0.11765f, 0.24313f, 0.21960f);
	else if (visualizationType == FIELD_TYPE_VALUE)
		glColor(110, 110, 75);
		//glColor3f(0.21960f, 0.11765f, 0.24313f);
		
	drawRect(GL_QUADS, border);

	glLineWidth(1.0f);
	glColor(100, 100, 100);
	drawRect(GL_LINE_LOOP, border);

	if (selected) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
		drawRect(GL_QUADS, border);
	}

	drawText();
};

bool gldConfigurator::addField(Box border) {
	ConfigField newField(border);
	int size = field.size();
	field.push_back(newField);
	return ((int)field.size() > size);
};
bool gldConfigurator::addField(int x1, int y1, int x2, int y2) {
	if (abs(x2 - x1) < 20 || abs(y2 - y1) < 20)
		return false;
	Box border;
	border.set(x1, y1, x2, y2);
	return addField(border);
};
bool gldConfigurator::addField() {
	Point mouse = input.mouse;
	return addField(mouse.x - FIELD_MIN_SIZE, mouse.y - FIELD_MIN_SIZE, mouse.x + FIELD_MIN_SIZE, mouse.y + FIELD_MIN_SIZE);
};
void gldConfigurator::deleteField(int which) {
	field[which] = field.back();
	field.pop_back();
};
void gldConfigurator::deleteAllFields() {
	field.clear();
};
unsigned gldConfigurator::fieldCount() {
	return field.size();
};
bool gldConfigurator::selectFieldUnderMouse() {
	for (int i = field.size() - 1; i >= 0; i--) {
		if (field[i].border.contains(input.mouse)) {
			selectedField = i;
			return true;
		}
	}
	return false;
};
bool gldConfigurator::selectFieldHandleUnderMouse() {
	for (int i = field.size() - 1; i >= 0; i--) {
		Point scaleHandle = point(field[i].border.right, field[i].border.bottom);
		if (distance((float)scaleHandle.x, (float)scaleHandle.y, (float)input.mouse.x, (float)input.mouse.y) <= (float)FIELD_SCALEHANDLE_SIZE) {
			selectedField = i;
			return true;
		}
	}
	return false;
};
void gldConfigurator::init() {
	loadPopupMenus();
};
void gldConfigurator::loadPopupMenus() {
	popupOnField.addItem(FIELD_NAME_TEXTURE);
	popupOnField.addItem(FIELD_NAME_MODEL);
	popupOnField.addItem(FIELD_NAME_VALUE);
	popupOnField.addItem(POPUPMENU_NULL);
	popupOnField.addItem(MENUITEM_FIELD_TO_SQUARE);
	popupOnField.addItem(MENUITEM_FIELD_DELETE);
	popupOnField.addItem(POPUPMENU_NULL);
	popupOnField.addItem(MENUITEM_FIELD_ADD);

	
	popupDefault.addItem(MENUITEM_FIELD_ADD);
	//popupDefault.addItem(MENUITEM_SAVE_RUN);
};
void gldConfigurator::mouseDown(mouseButton button) 
{
	string selectedMenuItem = POPUPMENU_NULL;
	switch (button) {
	case mbLeft:
		if (popupOnField.isActive())
			selectedMenuItem = popupOnField.selectedItemText();
		else if (popupDefault.isActive())
			selectedMenuItem = popupDefault.selectedItemText();
		if (selectedMenuItem != POPUPMENU_NULL) {
			if (selectedMenuItem == FIELD_NAME_TEXTURE)
				field[selectedField].visualizationType = 1;
			else
			if (selectedMenuItem == FIELD_NAME_MODEL)
				field[selectedField].visualizationType = 2;
			else
			if (selectedMenuItem == FIELD_NAME_VALUE)
				field[selectedField].visualizationType = 3;
			else
			if (selectedMenuItem == MENUITEM_FIELD_TO_SQUARE)
				field[selectedField].makeSquare();
			else
			if (selectedMenuItem == MENUITEM_FIELD_DELETE)
				deleteField(selectedField);
			else
			if (selectedMenuItem == MENUITEM_FIELD_ADD)
				addField();
			save();
		}
		else
		if (selectFieldHandleUnderMouse()) {
			resizingField = true;
		} else
		if (!addingField) {
			int x = snappedToGrid(input.mouse.x);
			int y = snappedToGrid(input.mouse.y);
			mouseClickPosition.set(x, y);
			addingField = true;
		}
		break;

	case mbMiddle:
		if (!movingField) {
			mouseClickPosition.set(input.mouse.x, input.mouse.y);
			if (selectFieldUnderMouse()) {
				movingField = true;
				movedFieldPosition = field[selectedField].border.getPosition();
			}
		}
		break;

	case mbRight:
		if (selectFieldUnderMouse()) {
			popupDefault.hide();
			popupOnField.show();
		}	
		else {
			popupDefault.show();
			popupOnField.hide();
		}
		break;
	}
};
void gldConfigurator::mouseUp(mouseButton button)
{
	switch (button) {
	case mbLeft:
		if (resizingField) {
			resizingField = false;
			selectedField = -1;
			save();
		} else
		if (addingField) {
			int x = snappedToGrid(input.mouse.x);
			int y = snappedToGrid(input.mouse.y);
			addField(mouseClickPosition.x, mouseClickPosition.y, x, y);
			addingField = false;
			save();
		}
		break;
	case mbMiddle:
		if (movingField) {
			movingField = false;
			selectedField = -1;
			save();
		}
		break;

	case mbRight:
		break;
	}
};
int gldConfigurator::snappedToGrid(int val) {
	int distance = val % GRID_SIZE;
	if (distance <= GRID_SNAP_DIST)
		val = GRID_SIZE * ( (int)val/GRID_SIZE );
	if (distance >= GRID_SIZE - GRID_SNAP_DIST)
		val = GRID_SIZE * ((int)val / GRID_SIZE + 1);
	return val;
};
void gldConfigurator::renderBackground() {
	glClearColor( 0.02f, 0.05f, 0.1f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f( 0.03f, 0.075f, 0.15f );
	for (int i = 0; i <= form.getHeight(); i += GRID_SIZE) {
		if (i % (GRID_SIZE * 3) == 0)
			glLineWidth(1.5f);
		glBegin(GL_LINES);
		glVertex2f(0, (GLfloat)i);
		glVertex2f((GLfloat)form.getWidth(), (GLfloat)i);
		glEnd();
		glLineWidth(1.0f);		
	}
	for (int i = 0; i <= form.getWidth(); i += GRID_SIZE) {
		if (i % (GRID_SIZE * 3) == 0)
			glLineWidth(1.5f);
		glBegin(GL_LINES);
		glVertex2f((GLfloat)i, 0);
		glVertex2f((GLfloat)i, (GLfloat)form.getHeight());
		glEnd();
		glLineWidth(1.0f);
	}
};
void gldConfigurator::render() {
	renderBackground();
	if (movingField) {
		movedFieldPosition.x += input.mouse.x - mouseClickPosition.x;
		movedFieldPosition.y += input.mouse.y - mouseClickPosition.y;
		int x = snappedToGrid(movedFieldPosition.x);
		int y = snappedToGrid(movedFieldPosition.y);
		field[selectedField].border.moveTo(x, y);
		mouseClickPosition = input.mouse;
	}
	if (resizingField) {
		int x = snappedToGrid(input.mouse.x);
		int y = snappedToGrid(input.mouse.y);
		field[selectedField].moveScaleHandle( x, y );
		mouseClickPosition = input.mouse;
	}
	if (!popupOnField.isActive() && !popupDefault.isActive() && !movingField && !resizingField) {
		selectedField = -1;
	}

	for (unsigned i = 0; i < field.size(); i++) {
		field[i].draw(i == selectedField);
	}
	for (unsigned i = 0; i < field.size(); i++) {
		field[i].drawScaleHandle();
	}
		
	if (addingField) {
		glLineWidth(1.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		int x = snappedToGrid(input.mouse.x);
		int y = snappedToGrid(input.mouse.y);
		glVertex2f((GLfloat)mouseClickPosition.x, (GLfloat)mouseClickPosition.y);
		glVertex2f((GLfloat)x, (GLfloat)mouseClickPosition.y);
		glVertex2f((GLfloat)x, (GLfloat)y);
		glVertex2f((GLfloat)mouseClickPosition.x, (GLfloat)y);
		glEnd();
	}
	popupOnField.draw();
	popupDefault.draw();
};
unsigned gldConfigurator::numberOfField() {
	return field.size();
};
bool gldConfigurator::save() {
	for (unsigned i = 0; i < field.size(); i++) {
		if (i < configLoader.field.size()) {
			configLoader.field[i].border.set(field[i].border);
			configLoader.fieldSetType(i, field[i].visualizationType);
		}
		else {
			ConfigFieldRecord addedField;
			addedField.border.set(field[i].border);
			addedField.type = field[i].visualizationType;
			configLoader.field.push_back(addedField);
		}
	}
	if (configLoader.field.size() > field.size())
		configLoader.field.resize(field.size());
	configLoader.save();
	return true;
};
bool gldConfigurator::load() {
	deleteAllFields();
	for (unsigned i = 0; i < configLoader.field.size(); i++) {
		addField(configLoader.field[i].border);
		field[i].visualizationType = configLoader.field[i].type;
	}
	return true;
};
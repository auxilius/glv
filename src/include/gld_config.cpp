#include "gld_config.h"




//MessageBox(0, "And text here", "MessageBox caption", MB_OK);

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
void ConfigField::moveScaleHandle(Point point) {
	if (point.x < border.left + FIELD_MIN_SIZE)
		point.x = border.left + FIELD_MIN_SIZE;
	border.right = point.x;
	if (point.y < border.top + FIELD_MIN_SIZE)
		point.y = border.top + FIELD_MIN_SIZE;
	border.bottom = point.y;
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
	glColor3f(0.556f, 0.556f, 0.556f);
	drawRect(GL_LINE_LOOP, border);

	if (selected) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
		drawRect(GL_QUADS, border);
	}

	if (pointDistance(input.mouse, point(border.right, border.bottom)) < FIELD_SCALEHANDLE_SIZE) {
		glColor3f(0.8f, 0.8f, 0.8f);
		glEnable(GL_POINT_SMOOTH);
		glPointSize((GLfloat)FIELD_SCALEHANDLE_SIZE*2);
		glBegin( GL_POINTS); glVertex2f(border.right, border.bottom); glEnd();
	}

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

void gldConfigurator::OnMouseDown(int x, int y, mouseButton Btn) {
	//M
};

bool gldConfigurator::addField(Box border) {
	ConfigField newField(border);
	int size = field.size();
	field.push_back(newField);
	return (field.size() > size);
};
bool gldConfigurator::addField(int x1, int y1, int x2, int y2) {
	if (abs(x2 - x1) < 20 || abs(y2 - y1) < 20)
		return false;
	ConfigField newField(x1, y1, x2, y2);
	int size = field.size();
	field.push_back(newField);
	return (field.size() > size);
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
		if (distance(scaleHandle.x, scaleHandle.y, input.mouse.x, input.mouse.y) <= FIELD_SCALEHANDLE_SIZE) {
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
		}
		else
		if (selectFieldHandleUnderMouse()) {
			resizingField = true;
		} else
		if (!addingField) {
			mouseClickPosition.set(input.mouse.x, input.mouse.y);
			addingField = true;
		}
		break;

	case mbMiddle:
		if (!movingField) {
			mouseClickPosition.set(input.mouse.x, input.mouse.y);
			if (selectFieldUnderMouse())
				movingField = true;
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
		} else
		if (addingField) {
			addField(mouseClickPosition.x, mouseClickPosition.y, input.mouse.x, input.mouse.y);
			addingField = false;
		}
		break;
	case mbMiddle:
		if (movingField) {
			movingField = false;
			selectedField = -1;
		}
		break;

	case mbRight:
		break;
	}
};
void gldConfigurator::render() {
	if (movingField) {
		field[selectedField].border.move(input.mouse.x - mouseClickPosition.x, input.mouse.y - mouseClickPosition.y);
		mouseClickPosition = input.mouse;
	}
	if (resizingField) {
		field[selectedField].moveScaleHandle(input.mouse);
		mouseClickPosition = input.mouse;
	}
	if (!popupOnField.isActive() && !popupDefault.isActive() && !movingField && !resizingField) {
		selectedField = -1;
	}

	
	for (unsigned i = 0; i < field.size(); i++) {
		field[i].draw(i == selectedField);
	}
		
	if (addingField) {
		glLineWidth(1.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(mouseClickPosition.x, mouseClickPosition.y);
		glVertex2f(input.mouse.x, mouseClickPosition.y);
		glVertex2f(input.mouse.x, input.mouse.y);
		glVertex2f(mouseClickPosition.x, input.mouse.y);
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
		if (i < configuration.field.size()) {
			configuration.field[i].border.set(field[i].border);
			configuration.fieldSetType(i, field[i].visualizationType);
		}
		else {
			FieldConfigRecord addedField;
			addedField.border.set(field[i].border);
			addedField.type = field[i].visualizationType;
			configuration.field.push_back(addedField);
		}
	}
	if (configuration.field.size() > field.size())
		configuration.field.resize(field.size());
	return true;
};
bool gldConfigurator::load() {
	deleteAllFields();
	for (unsigned i = 0; i < configuration.field.size(); i++) {
		addField(configuration.field[i].border);
		field[i].visualizationType = configuration.field[i].type;
	}
	return true;
};
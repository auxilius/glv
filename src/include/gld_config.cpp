#include "gld_config.h"


#define MENUITEM_FIELD_FOR_TEXTURE	"TEXTURE FIELD"
#define MENUITEM_FIELD_FOR_MESH		"MESH FIELD"
#define MENUITEM_FIELD_FOR_VALUE	"VALUE FIELD"
#define MENUITEM_FIELD_TO_SQUARE	"Set To Square"
#define MENUITEM_FIELD_DELETE		"Delete"
#define MENUITEM_FIELD_ADD			"Add Field"
#define MENUITEM_SAVE_RUN			"Save & Finish"

//MessageBox(0, "And text here", "MessageBox caption", MB_OK);

using namespace std;



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
	if (visualizationType != 0)
		glColor3f(0.11765f, 0.24313f, 0.21960f);
	else
		glColor3f(0.3f, 0.3f, 0.3f);
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
		glPointSize((GLfloat)FIELD_SCALEHANDLE_SIZE);
		glBegin( GL_POINTS); glVertex2f(border.right, border.bottom); glEnd();
	}

	double scale = (double)(border.width) / 20.0;
	if (scale > 25) scale = 25;
	if (border.height < 45) scale = 9;
	setFontSize(scale);
	glColor4f(0.95f, 0.95f, 0.95f, 0.95f);
	textPrintCentered(border.center.x, border.center.y, visualizationTypeCaption[visualizationType]);
};

void gldConfigurator::OnMouseDown(int x, int y, mouseButton Btn) {
	//M
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
	return addField(mouse.x - 20, mouse.y - 20, mouse.x + 20, mouse.y + 20);
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
	popupOnField.addItem(MENUITEM_FIELD_FOR_TEXTURE);
	popupOnField.addItem(MENUITEM_FIELD_FOR_MESH);
	popupOnField.addItem(MENUITEM_FIELD_FOR_VALUE);
	popupOnField.addItem(POPUPMENU_NULL);
	popupOnField.addItem(MENUITEM_FIELD_TO_SQUARE);
	popupOnField.addItem(MENUITEM_FIELD_DELETE);
	
	popupDefault.addItem(MENUITEM_FIELD_ADD);
	popupDefault.addItem(MENUITEM_SAVE_RUN);
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
			if (selectedMenuItem == MENUITEM_FIELD_FOR_TEXTURE)
				field[selectedField].visualizationType = 1;
			else
			if (selectedMenuItem == MENUITEM_FIELD_FOR_MESH)
				field[selectedField].visualizationType = 2;
			else
			if (selectedMenuItem == MENUITEM_FIELD_FOR_VALUE)
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
bool gldConfigurator::saveConfiguration() {
	ofstream stream;
	stream.open(FILE_CONFIG);
	if (stream.fail()) 
		MessageBox(0, L"Saving configuration failed", L"Cannot create configuration file, unknown error.", MB_OK);
	stream << form.left << " " << form.top << " " 
		   << form.width << " " << form.height << std::endl;
	stream << field.size() << endl;
	for (unsigned i = 0; i < field.size(); i++) {
		stream << field[i].border.top << " ";
		stream << field[i].border.right << " ";
		stream << field[i].border.bottom << " ";
		stream << field[i].border.left << " ";
		stream << field[i].visualizationType << std::endl;
	}
	stream.close();
	return true;
};
bool gldConfigurator::loadConfiguration() {
	ifstream stream;
	stream.open(FILE_CONFIG);
	if (stream.fail())
		return false;
	unsigned x;
	stream >> x >> x >> x >> x;
	unsigned numberOfFields = 0;
	stream >> numberOfFields;
	Box fieldBorder;
	unsigned visualizationType;
	for (unsigned i = 0; i < numberOfFields; i++) {
		stream >> fieldBorder.top;
		stream >> fieldBorder.right;
		stream >> fieldBorder.bottom;
		stream >> fieldBorder.left;
		stream >> visualizationType;
		if (addField(fieldBorder.left, fieldBorder.top, fieldBorder.right, fieldBorder.bottom) )
			field[i].visualizationType = visualizationType;
		else {
			deleteAllFields();
			MessageBox(0, L"Configuration loading failed", L"Cannot load configuration, configuration file may be corrupted.", MB_OK);
			return false;
		}
	}
	stream.close();
	return true;
};
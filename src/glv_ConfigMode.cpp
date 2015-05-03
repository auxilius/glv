#include "glv_ConfigMode.h"


#define FIELD_NAME_NONE				"-x-"
#define FIELD_NAME_ABR_NONE			"-x-"
#define FIELD_NAME_TEXTURE			"Texture Field"
#define FIELD_NAME_ABR_TEXTURE		"TEX"
#define FIELD_NAME_MODEL			"Mesh Field"
#define FIELD_NAME_ABR_MODEL		"MES"
#define FIELD_NAME_VALUE			"Variable Field"
#define FIELD_NAME_ABR_VALUE		"VAR"

#define MENUITEM_FIELD_SET_MODEL	"To Mesh Field"
#define MENUITEM_FIELD_SET_TEXTURE	"To Texture Field"
#define MENUITEM_FIELD_SET_VALUE	"To Variable Field"
#define MENUITEM_FIELD_DELETE		"Delete"
#define MENUITEM_FIELD_ADD_MODEL	"New Field - Mesh "
#define MENUITEM_FIELD_ADD_TEXTURE	"New Field - Texture"
#define MENUITEM_FIELD_ADD_VALUE	"New Field - Value"

#define GRID_SIZE				30
#define GRID_SNAP_DIST			5


using namespace std;

ConfigModeControl::ConfigModeControl(FieldManager * fManager) {
	addingField = false;
	movingField = false;
	resizingField = false;
	selectedField = -1;
	fieldManager = fManager;
}

bool ConfigModeControl::trySelectFieldUnderMouse() {
	selectedField = fieldManager->getFieldUnderMouse();
	return (selectedField != -1);
};

bool ConfigModeControl::trySelectFieldHandleUnderMouse() {
	selectedField = fieldManager->getFieldWithHandleUnderMouse();
	return (selectedField != -1);
};

void ConfigModeControl::init() {
	// menu - when clicked on field
	using namespace std::placeholders;
	popupOnField.OnItemClick = std::bind(&ConfigModeControl::onMenuClick, this, _1, _2);
	popupOnField.addItem(MENUITEM_FIELD_SET_MODEL);
	popupOnField.addItem(MENUITEM_FIELD_SET_TEXTURE);
	popupOnField.addItem(MENUITEM_FIELD_SET_VALUE);
	popupOnField.addItem(NULL_ITEM);
	popupOnField.addItem(MENUITEM_FIELD_DELETE);
	popupOnField.addItem(NULL_ITEM);
	popupOnField.addItem(MENUITEM_FIELD_ADD_MODEL);
	popupOnField.addItem(MENUITEM_FIELD_ADD_TEXTURE);
	popupOnField.addItem(MENUITEM_FIELD_ADD_VALUE);
	// menu - when clicked on empty space
	popupDefault.OnItemClick = std::bind(&ConfigModeControl::onMenuClick, this, _1, _2);
	popupDefault.addItem(MENUITEM_FIELD_ADD_MODEL);
	popupDefault.addItem(MENUITEM_FIELD_ADD_TEXTURE);
	popupDefault.addItem(MENUITEM_FIELD_ADD_VALUE);
};

void ConfigModeControl::mouseDown(mouseButton button) 
{
	if (popupDefault.isActive()) {
		popupDefault.onMouseDown(button);
		return;
	}
	if (popupOnField.isActive()) {
		popupOnField.onMouseDown(button);
		return;
	}

	if (button == mbLeft) {

		if ( trySelectFieldHandleUnderMouse() ) {
			resizingField = true;
		} 
		else 
		if (!addingField) {
			int x = snappedToGrid(input.mouse.x);
			int y = snappedToGrid(input.mouse.y);
			mouseClickPosition.set(x, y);
			addingField = true;
		}
	}
	else 
	if (button == mbMiddle) {
		if (!movingField) {
			mouseClickPosition.set(input.mouse.x, input.mouse.y);
			if ( trySelectFieldUnderMouse() ) {
				movingField = true;
				movedFieldPosition = fieldManager->fieldGetPosition(selectedField);
			}
		}
	}
	else
	if (button == mbRight) {

		if (addingField) {
			addingField = false;
		}
		else
		if ( trySelectFieldUnderMouse() ) {
			popupDefault.hide();
			popupOnField.showAtMousePosition();
		}	
		else {
			popupDefault.showAtMousePosition();
			popupOnField.hide();
		}
	}
};

void ConfigModeControl::mouseUp(mouseButton button)
{
	switch (button) {

	case mbLeft:
		if (resizingField) {
			resizingField = false;
			selectedField = -1;
			fieldManager->save();
		} else
		if (addingField) {
			addingField = false;
			int x = snappedToGrid(input.mouse.x);
			int y = snappedToGrid(input.mouse.y);
			if (fieldManager->addField(mouseClickPosition.x, mouseClickPosition.y, x, y))
				fieldManager->save();
		}
		break;

	case mbMiddle:
		if (movingField) {
			movingField = false;
			selectedField = -1;
			fieldManager->save();
		}
		break;

	case mbRight:
		break;
	}
};

int ConfigModeControl::snappedToGrid(int val) {
	int distance = val % GRID_SIZE;
	if (distance <= GRID_SNAP_DIST)
		val = GRID_SIZE * ( (int)val/GRID_SIZE );
	if (distance >= GRID_SIZE - GRID_SNAP_DIST)
		val = GRID_SIZE * ((int)val / GRID_SIZE + 1);
	return val;
};

Point ConfigModeControl::snappedToGrid(const Point &point) {
	Point result;
	result.set( snappedToGrid(point.x), snappedToGrid(point.y) );
	return result; 
};

void ConfigModeControl::renderBackground() {
	glClearColor( 0.05f, 0.1f, 0.02f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f( 0.075f, 0.15f, 0.03f );
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

void ConfigModeControl::render() {
	renderBackground();
	if (movingField) {
		movedFieldPosition.x += input.mouse.x - mouseClickPosition.x;
		movedFieldPosition.y += input.mouse.y - mouseClickPosition.y;
		Point pos = snappedToGrid(movedFieldPosition);
		fieldManager->fieldMoveTo(selectedField, pos.x, pos.y);
		mouseClickPosition = input.mouse;
	}
	if (resizingField) {
		Point pos = snappedToGrid(input.mouse);
		fieldManager->fieldMoveHandleTo(selectedField, pos.x, pos.y);
		mouseClickPosition = input.mouse;
	}
	if (!popupOnField.isActive() && !popupDefault.isActive() && !movingField && !resizingField) {
		selectedField = -1;
	}

	for (unsigned i = 0; i < fieldManager->fieldCount(); i++)
		renderField(i, i==selectedField);
		
	if (addingField) {
		int dx = abs(mouseClickPosition.x - input.mouse.x);
		int dy = abs(mouseClickPosition.y - input.mouse.y);
		if (dx > FIELD_MIN_SIZE_TO_SHOW && dy > FIELD_MIN_SIZE_TO_SHOW)
		{
			glLineWidth(1.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			
			int x = snappedToGrid(input.mouse.x);
			int y = snappedToGrid(input.mouse.y);
			if (dx < FIELD_MIN_SIZE) 
				x = mouseClickPosition.x + FIELD_MIN_SIZE;
			if (dy < FIELD_MIN_SIZE) 
				y = mouseClickPosition.y + FIELD_MIN_SIZE;
			glBegin(GL_LINE_LOOP);
			glVerticeSquare(
				mouseClickPosition.x, mouseClickPosition.y,
				x, y
			);
			glEnd();
		}
	}
	popupOnField.draw();
	popupDefault.draw();
};

void ConfigModeControl::renderField(const int i, bool selected) {
	
	Box border = fieldManager->fieldGetBorder(i);
	unsigned type = fieldManager->fieldGetType(i);
	
	// draw rectangle with color of assigned type
	drawBox(&border, *fieldTypeColor(type));
	
	// render border
	glLineWidth(1.0f);
	drawBox(&border, clGray40, false);
	
	// highlight overlay
	if (selected) {
		Color highlit = {1.0f, 1.0f, 1.0f, 0.12f};
		drawBox(&border, highlit);
	}
	
	// field text on the top
	glColor4f(0.95f, 0.95f, 0.95f, 0.95f);
	if (border.width < 170) // smallest caption, only abbriviation
		fontHuge->textOut(border.center.x, border.center.y - fontHuge->getHeight() / 2, fieldTypeName(type, true), faCenter);
	else if (border.width < 250) // modest caption
		fontLarge->textOut(border.center.x, border.center.y - fontLarge->getHeight() / 2, fieldTypeName(type), faCenter);
	else // super big caption
		fontHuge->textOut(border.center.x, border.center.y - fontHuge->getHeight() / 2, fieldTypeName(type), faCenter);
	
	// render scaling handle
	if (pointDistance(input.mouse, point(border.right, border.bottom)) < FIELD_SCALEHANDLE_SIZE) {
		Point position(border.right, border.bottom);
		drawPoint(&position, clWhite, FIELD_SCALEHANDLE_SIZE * 2);
	}
};

void ConfigModeControl::onMenuClick(int itemID, std::string itemCaption) {

	if (itemCaption == MENUITEM_FIELD_SET_TEXTURE)
		fieldManager->fieldSetType(selectedField, FIELD_TYPE_TEXTURE);

	else if (itemCaption == MENUITEM_FIELD_SET_MODEL)
		fieldManager->fieldSetType(selectedField, FIELD_TYPE_MODEL);

	else if (itemCaption == MENUITEM_FIELD_SET_VALUE)
		fieldManager->fieldSetType(selectedField, FIELD_TYPE_VALUE);

	else if (itemCaption == MENUITEM_FIELD_DELETE)
		fieldManager->fieldDelete(selectedField);

	else if (itemCaption == MENUITEM_FIELD_ADD_MODEL)
		fieldManager->addFieldAtMousePos(FIELD_TYPE_MODEL);

	else if (itemCaption == MENUITEM_FIELD_ADD_TEXTURE)
		fieldManager->addFieldAtMousePos(FIELD_TYPE_TEXTURE);

	else if (itemCaption == MENUITEM_FIELD_ADD_VALUE)
		fieldManager->addFieldAtMousePos(FIELD_TYPE_VALUE);

	fieldManager->save();

	popupDefault.hide();
	popupOnField.hide();
};

Color * ConfigModeControl::fieldTypeColor(const int &type) {
	if (type == FIELD_TYPE_NONE)
		return &clGray70;
	if (type == FIELD_TYPE_MODEL)
		return &clConfBoxMes;
	if (type == FIELD_TYPE_TEXTURE)
		return &clConfBoxTex;
	if (type == FIELD_TYPE_VALUE)
		return &clConfBoxVal;
};

char * ConfigModeControl::fieldTypeName(const int &type, const bool abriviation) {

	if (type == FIELD_TYPE_NONE) {
		if (abriviation)
			return FIELD_NAME_ABR_NONE;
		else
			return FIELD_NAME_NONE;
	}
	if (type == FIELD_TYPE_TEXTURE) {
		if (abriviation)
			return FIELD_NAME_ABR_TEXTURE;
		else
			return FIELD_NAME_TEXTURE;
	}

	if (type == FIELD_TYPE_MODEL) {
		if (abriviation)
			return FIELD_NAME_ABR_MODEL;
		else
			return FIELD_NAME_MODEL;
	}

	if (type == FIELD_TYPE_VALUE) {
		if (abriviation)
			return FIELD_NAME_ABR_VALUE;
		else
			return FIELD_NAME_VALUE;
	}

	return "";
};


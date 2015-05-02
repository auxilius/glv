#include "glv_FieldManager.h"


FieldManager::FieldManager() {
	actualFile = "";
};

unsigned FieldManager::fieldCount(void) {
	return field.size();
};

bool FieldManager::isEmpty() {
	return field.size() <= 0;
};

bool FieldManager::isBlank() {
	for (unsigned i = 0; i < field.size(); i++)
		if (field[i]->getType() != FIELD_TYPE_NONE)
			return false;
	return true;
};

bool FieldManager::addField(Box border, const unsigned &type) {
	// if any dimension is under treshold, dont add it
	if (border.width < FIELD_MIN_SIZE_TO_SHOW || border.height < FIELD_MIN_SIZE_TO_SHOW)
		return false;
	// if width is too small, change it
	if (border.width < FIELD_MIN_SIZE)
		border.right = border.left + FIELD_MIN_SIZE;
	// if height is too small, change it
	if (border.height < FIELD_MIN_SIZE)
		border.bottom = border.top + FIELD_MIN_SIZE;
	field.push_back( new Field(border, type) );
	return true;
};

bool FieldManager::addField(int x1, int y1, int x2, int y2, const unsigned &type) {
	Box newBorder(x1, y1, x2, y2);
	return addField(newBorder, type);
};

void FieldManager::addFieldAtMousePos(const unsigned &type) {
	Point mouse = input.mouse;
	Box border( mouse.x-FIELD_MIN_SIZE, mouse.y-FIELD_MIN_SIZE, mouse.x+FIELD_MIN_SIZE, mouse.y+FIELD_MIN_SIZE );
	field.push_back( new Field(border,type) );
};

bool FieldManager::fieldIsValid(const int &id) {
	return (id >= 0) && (id < (int)field.size());
};

void FieldManager::fieldDelete(const int &id) {
	if (fieldIsValid(id)) {
		field[id] = field.back();
		field.pop_back();
	}
};

void FieldManager::fieldSetType(const int &id, const short &fType) {
	if (fieldIsValid(id))
		field[id]->setType(fType);
};

void FieldManager::fieldSetParams(const int &id, FieldParams newParams) {
	if (fieldIsValid(id))
		field[id]->params = newParams;
};

Point FieldManager::fieldGetPosition(const int &id) {
	if (fieldIsValid(id))
		return field[id]->getBorder().getPosition();
	return Point();
};

Box FieldManager::fieldGetBorder(const int &id) {
	if (fieldIsValid(id))
		return field[id]->getBorder();
	return Box();
};

short FieldManager::fieldGetType(const int &id) {
	if (fieldIsValid(id))
		return field[id]->getType();
	return 0;
};

FieldParams FieldManager::getFieldParams(const int &id) {
	if (fieldIsValid(id))
		return field[id]->params;
	return FieldParams();
};

void FieldManager::fieldMoveTo(const int &id, const int &x, const int &y) {
	if (fieldIsValid(id))
		field[id]->moveTo(x, y);
};

void FieldManager::fieldMoveHandleTo(const int &id, const int &x, const int &y) {
	if (fieldIsValid(id))
		field[id]->moveScaleHandle( x, y );
};

void FieldManager::fieldResize(const int &id, const int &w, const int &h) {
	if (fieldIsValid(id))
		field[id]->resize( w, h );
};

void FieldManager::allFieldsDelete(void) {
	field.clear();
};

int FieldManager::getFieldUnderMouse(void) {
	for (int i = field.size()-1; i>=0; i--) {
		if (field[i]->getBorder().contains(input.mouse))
			return i;
	}
	return -1;
};

int FieldManager::getFieldWithHandleUnderMouse(void) {
	Point mouse = input.mouse;
	Box *border;
	for (int i = field.size()-1; i>=0; i--) {
		border = &field[i]->getBorder();
		double dist = distance((float)border->right, (float)border->bottom, (float)mouse.x, (float)mouse.y);
		if (dist <= FIELD_SCALEHANDLE_SIZE)
			return i;
	}
	return -1;
};

bool FieldManager::load(std::string fileName) {
	if ( !fileExists(fileName.c_str()) ) {
		if ( fileCreate(fileName.c_str()) ) {
			field.clear();
			actualFile = fileName;
			return true;
		}
	}
	std::ifstream inStream ( fileName );
	if (inStream.fail()) {
		MessageBox(0, "Error opening config file.", "GLD - loading error", MB_OK | MB_ICONWARNING);
		return false;
	}
	std::string version;
	getline(inStream, version);
	actualFile = "";
	// READ - SKIPPING WIDTH, HEIGHT, X, Y
	inStream >> form.left >> form.top >> form.right >> form.bottom;
	SetWindowPos(windowHandle, 0, form.left, form.top, form.width, form.height, SWP_NOZORDER|SWP_NOACTIVATE);
	// READ - NUMBER OF FIELDS
	field.clear();
	unsigned count = 0;
	inStream >> count;
	// READ - FIELDS
	Field *newField;
	for (unsigned i = 0; i < count; i++) {
		newField = new Field(inStream, version);
		field.push_back(newField);
	}
	inStream.close();
	actualFile = fileName;
	return true;
};

bool FieldManager::save() {
	if (actualFile == "")
		return false;
	std::ofstream stream ( actualFile );
	if (stream.fail()) {
		std::cout << "ERROR saving config file " << actualFile << std::endl;
		MessageBox(0, "Cannot create configuration file, location may be write protected.", "Saving configuration failed", MB_OK | MB_ICONWARNING);
		return false;
	}
	stream << CONFIG_FILE_VERSION << std::endl;
	stream << form.left << " " << form.top << " " << form.right << " " << form.bottom << std::endl;
	stream << field.size() << std::endl;
	for (unsigned i = 0; i < field.size(); i++)
		field[i]->saveToStream(stream);
	stream.close();
	return true;
};

/*
void FieldManager::debugOut() {
	std::cout << "-------------------------------------" << std::endl;
	std::cout << "Configuration:" << std::endl;
	for (unsigned i = 0; i < field.size(); i++) {
		Field * f = field[i];
		Box * border = &f->getBorder();
		std::cout << "Field " << i << " - " << border->left << " " << border->top << " " << border->right << " " << border->bottom << std::endl;
		std::cout << "  type: " << f->getType() << std::endl;
		std::cout << "  param_i: ";
		for (unsigned x = 0; x < f->param_i.size(); x++)
			std::cout << f->param_i[x] << " ";
		std::cout << std::endl;
		std::cout << "  param_d: ";
		for (unsigned x = 0; x < f->param_d.size(); x++)
			std::cout << f->param_d[x] << " ";
		std::cout << std::endl;
		std::cout << "  param_str: '" << f->param_str << "'" << std::endl;
	}
}; /**/

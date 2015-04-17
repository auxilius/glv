#include "glv_Field.h"
#include "gld_drawing.h"
#include <iomanip>




void FieldParams::saveToStream(std::ofstream & stream) {
	// number of int parameters followed by them
	stream << iParam.size() << " ";
	for (unsigned i = 0; i < iParam.size(); i++)
		stream << iParam[i] << " ";
	// number of float parameters followed by them
	stream << dParam.size() << " ";
	for (unsigned i = 0; i < dParam.size(); i++)
		stream << std::fixed << std::setprecision(5) << dParam[i] << " ";
	// save string parameter
	if (strParam.empty()) 
		stream << "-";
	else 
		stream << strParam.c_str();
};

void FieldParams::loadFromStream(std::ifstream & stream) {
	// load int parameters
	iParam.clear();
	unsigned paramCount = 0;
	stream >> paramCount;
	int newParam_i;
	for (unsigned i = 0; i < paramCount; i++) {
		stream >> newParam_i;
		iParam.push_back(newParam_i);
	}
	// load float parameters
	dParam.clear();
	stream >> paramCount;
	double newParam_d;
	for (unsigned i = 0; i < paramCount; i++) {
		stream >> newParam_d;
		dParam.push_back(newParam_d);
	}
	// load string parameter and delete first space character
	std::getline(stream, strParam);
	strParam.erase(strParam.begin());
};

void FieldParams::clear() {
	iParam.clear();
	dParam.clear();
	strParam = "";
};




Field::Field(const Box &cBorder, const short &fType) {
	border.set(cBorder);
	type = fType;
};

Field::Field(int x1, int y1, int x2, int y2, const short &fType) {
	if (abs(x2 - x1) < FIELD_MIN_SIZE)
		x2 = x1 + FIELD_MIN_SIZE;
	if (abs(y2 - y1) < FIELD_MIN_SIZE)
		y2 = y1 + FIELD_MIN_SIZE;
	border.set(x1, y1, x2, y2);
	type = fType;
};

Field::Field() {
	type = 0;
};

Field::Field(std::ifstream & stream) {
	loadFromStream(stream);
};

void Field::makeSquare() {
	int size = max(border.width, border.height);
	border.setSize(size, size);
};

bool Field::setType(const short &newType) {
	if (newType == type)
		return false;
	type = newType;
	params.clear();
	return true;
};

void Field::setBorder(const Box &newBorder) {
	border.set(newBorder);
	return;
};

void Field::moveTo(const int &x, const int &y) {
	border.moveTo(x,y);
};

void Field::resize(const int &w, const int &h) {
	border.setSize(w,h);
};

short Field::getType() {
	return type;
};

Box Field::getBorder() {
	return border;
};

void Field::moveScaleHandle(int x, int y) {
	if (x < border.left + FIELD_MIN_SIZE)
		x = border.left + FIELD_MIN_SIZE;
	border.right = x;
	if (y < border.top + FIELD_MIN_SIZE)
		y = border.top + FIELD_MIN_SIZE;
	border.bottom = y;
};

void Field::moveScaleHandle(Point point) {
	moveScaleHandle(point.x, point.y);
};

void Field::saveToStream(std::ofstream & stream) {
	// position of the field
	stream << border.left << " " << border.top << " " << border.right << " " << border.bottom << " ";
	// type of the field
	stream << type << " ";
	// save parameters
	params.saveToStream(stream);
	// finish line
	stream << std::endl;
};

void Field::loadFromStream(std::ifstream & stream) {
	// position of the field
	stream >> border.left >> border.top >> border.right >> border.bottom;
	// load type of the field
	stream >> type;
	// load parameters
	params.loadFromStream(stream);
};


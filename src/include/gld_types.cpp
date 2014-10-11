#include "gld_types.h"


inputManager input;
Box canvas;
IOBox form;
FieldConfig configuration;
std::string SAVE_PATH = "";


void Point::set(int sx, int sy) {
	x = sx; y = sy;
};
Point point(int x, int y) {
	Point result;
	result.set(x, y);
	return result;
};

double distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
};
double pointDistance(Point p1, Point p2) {
	return sqrt(pow((long double)(p1.x - p2.x), 2) + pow((long double)(p1.y - p2.y), 2));
};

Point Box::getPosition() {
	Point result;
	result.set(left, top);
	return result;
};
Point Box::getCenterPosition() {
	Point result;
	result.set((left + right) / 2, (top + bottom) / 2);
	return result;
};
void Box::set(int x1, int y1, int x2, int y2) 
{
	if (x1 > x2)
		std::swap(x1, x2);
	if (y1 > y2)
		std::swap(y1, y2);
	top = y1; left = x1; bottom = y2; right = x2;
};
void Box::set(int x, int y, int size)
{
	left = x;
	top = y; 
	right = x+size;
	bottom = y+size;
};
void Box::set(Box border) {
	set(border.left, border.top, border.right, border.bottom);
};
bool Box::contains(int x, int y) 
{
	return (x > left && x <= right && y > top && y <= bottom);
};
bool Box::contains(Point position)
{
	return (position.x >= left && position.x <= right && position.y >= top && position.y <= bottom);
};
void Box::move(int vx, int vy) 
{
	top += vy; bottom += vy; left += vx; right += vx;
};
void Box::moveTo(int x, int y) 
{
	int width = right - left;
	int height = bottom - top;
	top = y; bottom = y + height;
	left = x; right = x + width;
};
int Box::getWidth() 
{
	return right - left;
};
int Box::getHeight() 
{
	return bottom - top;
};
void Box::setWidth(int newWidth)
{
	if (newWidth < 0) 
		return;
	right = left + newWidth;
};
void Box::setHeight(int newHeight)
{
	if (newHeight < 0)
		return;
	bottom = top + newHeight;
};
void Box::setSize(int newWidth, int newHeight)
{
	setWidth(newWidth);
	setHeight(newHeight);
};


void IOBox::save(std::ofstream & stream) {
	stream << left << " " << top << " " << right << " " << bottom << " ";

};
void IOBox::load(std::ifstream & stream) {
	stream >> left >> top >> right >> bottom;
};


FieldConfigRecord::FieldConfigRecord() {
	type = 0;
};
void FieldConfigRecord::save(std::ofstream & stream) {
	border.save(stream);
	stream << type << " ";
	stream << param.size() << " ";
	for (unsigned i = 0; i < param.size(); i++)
		stream << param[i] << " ";
	if (paramText.empty()) stream << "-";
	else stream << paramText.c_str();
	stream << std::endl;
};
void FieldConfigRecord::load(std::ifstream & stream) {
	border.load(stream);
	stream >> type;
	param.clear();
	unsigned paramCount = 0;
	stream >> paramCount;
	int newParam;
	for (unsigned i = 0; i < paramCount; i++) {
		stream >> newParam;
		param.push_back(newParam);
	}
	std::getline(stream, paramText);
	paramText.erase(paramText.begin());
};

bool FieldConfig::valid() {
	if (field.size() > 0) {
		for (unsigned i = 0; i < field.size(); i++)
			if (field[i].type != FIELD_TYPE_NONE)
				return true;
	}
	return false;
};
void FieldConfig::save() {
	if (!loaded)
		return;
	std::ofstream stream;
	stream.open(pathToFile(FILE_CONFIG));
	if (stream.fail())
		MessageBox(0, L"Cannot create configuration file, unknown error.", L"Saving configuration failed", MB_OK);
	form.save(stream);
	stream << std::endl;
	stream << field.size() << std::endl;
	for (unsigned i = 0; i < field.size(); i++)
		field[i].save(stream);
	stream.close();
};
void FieldConfig::load() {
	loaded = true;
	field.clear();
	std::ifstream stream;
	stream.open(pathToFile(FILE_CONFIG));
	if (stream.fail())
		return;
	unsigned x;
	stream >> x >> x >> x >> x;
	unsigned count = 0;
	stream >> count;
	FieldConfigRecord newField;
	for (unsigned i = 0; i < count; i++) {
		newField.load(stream);
		field.push_back(newField);
	}
	stream.close();
};
void FieldConfig::clear() {
	field.clear();
};
bool FieldConfig::fieldSetType(int f, int t) {
	if (f < field.size() && f >= 0) {
		if (field[f].type == t)
			return false;
		else {
			field[f].param.clear();
			field[f].paramText = "";
			field[f].type = t;
			return true;
		}
	}
	return false;
};
void FieldConfig::debugOut() {
	std::cout << "-------------------------------------" << std::endl;
	std::cout << "Configuration:" << std::endl;
	for (unsigned i = 0; i < field.size(); i++) {
		FieldConfigRecord * f = &field[i];
		std::cout << "Field " << i << " - " << f->border.left << " " << f->border.top << " " << f->border.right << " " << f->border.bottom << std::endl;
		std::cout << "  type: " << f->type << std::endl;
		std::cout << "  text: '" << f->paramText << "'" << std::endl;
	}
};


bool fileExists(char* fileName) {
	std::ifstream infile(fileName);
	bool result = infile.good();
	infile.close();
	return result;
};
bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; 
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true; 
	return false;
}


char * pathToFile(char* fname) {
	std::string path = SAVE_PATH + fname;
	char * result = stringToChar(path);
	return result;
};


char * stringToChar(std::string str) {
	char * writable = new char[str.size() + 1];
	std::copy(str.begin(), str.end(), writable);
	writable[str.size()] = '\0';
	return writable;
};


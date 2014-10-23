#include "gld_types.h"


inputManager input;
Box canvas;
IOBox form;
ConfigurationLoader configLoader;
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


ConfigFieldRecord::ConfigFieldRecord() {
	type = 0;
};
void ConfigFieldRecord::save(std::ofstream & stream) {
	border.save(stream);
	stream << type << " ";
	stream << param_i.size() << " ";
	for (unsigned i = 0; i < param_i.size(); i++)
		stream << param_i[i] << " ";
	stream << param_d.size() << " ";
	for (unsigned i = 0; i < param_d.size(); i++)
		stream << param_d[i] << " ";
	if (param_str.empty()) stream << "-";
	else stream << param_str.c_str();
	stream << std::endl;
};
void ConfigFieldRecord::load(std::ifstream & stream) {
	border.load(stream);
	stream >> type;
	param_i.clear();
	unsigned paramCount = 0;
	stream >> paramCount;
	int newParam_i;
	for (unsigned i = 0; i < paramCount; i++) {
		stream >> newParam_i;
		param_i.push_back(newParam_i);
	}
	param_d.clear();
	stream >> paramCount;
	double newParam_d;
	for (unsigned i = 0; i < paramCount; i++) {
		stream >> newParam_d;
		param_d.push_back(newParam_d);
	}
	std::getline(stream, param_str);
	param_str.erase(param_str.begin());
};

bool ConfigurationLoader::valid() {
	if (field.size() > 0) {
		for (unsigned i = 0; i < field.size(); i++)
			if (field[i].type != FIELD_TYPE_NONE)
				return true;
	}
	return false;
};
void ConfigurationLoader::save() {
	if (!loaded)
		return;
	std::ofstream stream;
	stream.open(pathToFile(FILE_CONFIG));
	if (stream.fail()) {
		MessageBox(0, L"Cannot create configuration file, unknown error.", L"Saving configuration failed", MB_OK);
	}
	stream << CONFIG_FILE_VERSION << std::endl;
	form.save(stream);
	stream << std::endl;
	stream << field.size() << std::endl;
	for (unsigned i = 0; i < field.size(); i++)
		field[i].save(stream);
	stream.close();
};

bool ConfigurationLoader::load() {
	loaded = true;
	field.clear();
	// OPENING FILE
	std::ifstream inStream;
	inStream.open(pathToFile(FILE_CONFIG));
	if (inStream.fail())
		return false;
	std::string version;
	getline(inStream, version);
	if (version != CONFIG_FILE_VERSION) {
		MessageBox(0, L"Version of configuration file is out of date. File will be replaced.", L"Loading configuration failed", MB_OK | MB_ICONWARNING);
		return false;
	}
	// READ - SKIPPING WIDTH, HEIGHT, X, Y
	form.load(inStream);
	// READ - NUMBER OF FIELDS
	unsigned count = 0;
	inStream >> count;
	// READ - FIELDS
	ConfigFieldRecord newField;
	for (unsigned i = 0; i < count; i++) {
		newField.load(inStream);
		field.push_back(newField);
	}
	inStream.close();
	return true;
};
void ConfigurationLoader::clear() {
	field.clear();
};
bool ConfigurationLoader::fieldSetType(int f, int t) {
	if (f < field.size() && f >= 0) {
		if (field[f].type == t)
			return false;
		else {
			field[f].param_i.clear();
			field[f].param_d.clear();
			field[f].param_str = "";
			field[f].type = t;
			return true;
		}
	}
	return false;
};
void ConfigurationLoader::debugOut() {
	std::cout << "-------------------------------------" << std::endl;
	std::cout << "Configuration:" << std::endl;
	for (unsigned i = 0; i < field.size(); i++) {
		ConfigFieldRecord * f = &field[i];
		std::cout << "Field " << i << " - " << f->border.left << " " << f->border.top << " " << f->border.right << " " << f->border.bottom << std::endl;
		std::cout << "  type: " << f->type << std::endl;
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


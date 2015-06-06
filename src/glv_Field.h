#pragma once
#ifndef GLV_H_CONFIG_FIELD 
#define GLV_H_CONFIG_FIELD

#include "gld_types.h"

struct FieldParams {
	std::vector<int> iParam;
	std::vector<double> dParam;
	std::vector<char> cParam;
	std::string strParam;
	void saveToStream(std::ofstream & stream);
	void loadFromStream(std::ifstream & stream, std::string &version);
	void clear();
};

class Field {
public:
	Field();
	Field(std::ifstream & stream, std::string version);
	Field(const Box &cBorder, const short &fType = GLV_FIELD_TYPE_NONE);
	Field(int x1, int y1, int x2, int y2, const short &fType = GLV_FIELD_TYPE_NONE);

	void moveScaleHandle(Point point);
	void moveScaleHandle(int x, int y);
	void makeSquare();

	bool setType(const short &newType);
	void setBorder(const Box &newBorder);
	void moveTo(const int &x, const int &y);
	void resize(const int &w, const int &h);

	short getType();
	Box getBorder();

public:
	FieldParams params;
	void saveToStream(std::ofstream & stream);
	void loadFromStream(std::ifstream &stream, std::string &version);

private:
	short type;
	Box border;
};

#endif
#pragma once
#ifndef GLV_H_FIELD_MANAGER
#define GLV_H_FIELD_MANAGER

#include <vector>
#include "glv_Field.h"


class FieldManager {
public:
	FieldManager();
	unsigned fieldCount(void);
	bool isEmpty();
	bool isBlank();
public: // field constructors
	bool addField(Box border, const unsigned &type = GLV_FIELD_TYPE_NONE );
	bool addField(int x1, int y1, int x2, int y2, const unsigned &type = GLV_FIELD_TYPE_NONE );
	void addFieldAtMousePos( const unsigned &type = GLV_FIELD_TYPE_NONE );
public: //  field destructors
	void fieldDelete(const int &id);
	void allFieldsDelete(void);
public: // field getters
	bool fieldIsValid(const int &id);
	int getFieldUnderMouse(void);
	int getFieldWithHandleUnderMouse(void);
	Point fieldGetPosition(const int &id);
	Box fieldGetBorder(const int &id);
	short fieldGetType(const int &id);
	FieldParams getFieldParams(const int &id);
public: // field setters
	void fieldMoveTo(const int &id, const int &x, const int &y);
	void fieldMoveHandleTo(const int &id, const int &x, const int &y);
	void fieldResize(const int &id, const int &w, const int &h);
	void fieldSetType(const int &id, const short &fType);
	void fieldSetParams(const int &id, FieldParams newParams);

public: 
	bool load(std::string fileName);
	bool save();

private:
	std::string actualFile;
	std::vector<Field*> field;

};


#endif
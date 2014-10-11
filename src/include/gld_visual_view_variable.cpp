#include <iostream>
#include <iomanip>
#include <sstream>
#include "gld_visual_view_variable.h"
#include "gld_drawing.h"


// SINGLE DATA
vwSingleData::vwSingleData() {
	error = false;
	data = NULL;
	format.param = 0;
	format.type = '\0';
};
std::string vwSingleData::getString() {
	std::stringstream result;
	if (format.param != 0) {
		result
			<< std::setprecision(format.param)
			<< std::fixed
			<< std::boolalpha;
	}
	try {
		if (data == NULL)
			result << "N/A";
		else
		if (format.type == 'i')
			result << *(int*)data;
		else
		if (format.type == 'u')
			result << *(unsigned*)data;
		else
		if (format.type == 'f')
			result << *(float*)data;
		else
		if (format.type == 'd')
			result << *(double*)data;
		else
		if (format.type == 'c')
			result << *(char*)data;
		else
		if (format.type == 'b')
			result << *(bool*)data;
		else
		if (format.type == 's')
			result << (char*)data;
		else
			result << "%" << format.type;
	}
	catch (...) {
		if (!error)
			MessageBox(0, L"Cannot read memory, poited value was probably ereased.", L"Memory read error", MB_OK);
		result << "err";
		error = true;
	}
	return result.str();
};

// LINE
vwFormat vwLine::readFormat(const char * formatString) {
	vwFormat result;
	unsigned i = 0;
	if (formatString[0] == '%')
		i = 1;
	if (formatString[i] >= '0' && formatString[i] <= '9') {
		result.param = (formatString[i] - '0');
		i++;
	}
	result.type = formatString[i];
	return result;
};
void vwLine::create(std::string format, void* data[]) {
	unsigned data_it = 0;
	std::string stringPart = "";
	for (std::string::size_type i = 0; i < format.size(); ++i) {
		if (format[i] == '%') {
			text.push_back(stringPart);
			stringPart = "";
			i++;
			vwSingleData V;
			while (format[i] >= '0' && format[i] <= '9') {
				V.format.param = format[i] - '0';
				i++;
			}
			V.data = data[data_it];
			data_it++;
			V.format.type = format[i];
			value.push_back(V);
		}
		else
			stringPart = stringPart + format[i];
	}
	text.push_back(stringPart);
};
void vwLine::printArray(void *data[], int startPosition, int length, const char * singleFormat) {
	unsigned data_it = 0;
	text.push_back("\t");
	vwFormat format = readFormat(singleFormat);
	for (unsigned i = 0; i < length; i++) {
		vwSingleData SD;
		SD.data = data[i];
		SD.format = format;
		value.push_back(SD);
	}
};
char * vwLine::getArray() {
	std::string out = ": ";
	for (unsigned i = 0; i < value.size(); i++) {
		out = out + value[i].getString() + " ";
	}
	char * result = new char[out.length() + 1];
	strcpy(result, out.c_str());
	return result;
};
char * vwLine::getText() {
	if (text.size() > 0 && text[0] == "\t")
		return getArray();
	std::string out = "";
	for (unsigned i = 0; i < text.size(); i++) {
		out = out + text[i];
		if (i < value.size())
			out = out + value[i].getString();
	}
	char * result = new char[out.length() + 1];
	strcpy(result, out.c_str());
	return result;
};
void vwLine::setText(std::string lineText) {
	text.push_back(lineText);
};

// ITEM
void vwItem::addLine(vwLine newLine) {
	line.push_back(newLine);
};
int vwItem::render(int x, int y) {
	const int lineHeight = getTextHeight("X") + 5;
	int yy = y;
	for (unsigned i = 0; i < line.size(); i++) {
		textPrint(x, yy, line[i].getText());
		yy += lineHeight;
	}
	return yy;
};


// VIEW
void VariableView::draw() {
	glColor3f(0.8f, 0.8f, 0.8f);
	setFontSize(8);
	int y = border.top + 5;
	int x = border.left + 5;
	for (unsigned i = 0; i < itemNumber.size(); i++) {
		if (itemNumber[i] < itemList->size())
			y = itemList->operator[](itemNumber[i]).render(x, y);
	}
};
void VariableView::setItem(unsigned num, bool state) {
	if (std::find(itemNumber.begin(), itemNumber.end(), (unsigned)num) == itemNumber.end()) {
		if (state == true)
			itemNumber.push_back((unsigned)num);
	}
	else {
		itemNumber.erase(
			std::remove(itemNumber.begin(), itemNumber.end(), (unsigned)num),
			itemNumber.end());
	}
};
void VariableView::setItemList(std::vector<vwItem> * list) {
	itemList = list;
};
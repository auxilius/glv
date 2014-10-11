#ifndef __H_GLD_VIEW_VARIABLE
#define __H_GLD_VIEW_VARIABLE

#include "gld_visual_view.h"

struct vwFormat {
	char type;
	short param;
};
struct vwSingleData {
private:
	bool error;
public:
	vwSingleData();
	void * data;
	vwFormat format;
	std::string getString();
};

class vwLine {
private:
	std::vector<std::string> text;
	std::vector<vwSingleData> value;
	vwFormat readFormat(const char * formatString);
	char * getArray();
public:
	char * getText();
	void setText(std::string format);
	void create(std::string format, void* data[]);
	void printArray(void * data[], int startPosition, int length, const char * singleFormat);
};

class vwItem {
private:
	std::vector<vwLine> line;
public:
	void addLine(vwLine newLine);
	int render(int x, int y);
};

class VariableView : public View {
private:
	std::vector<vwItem> *itemList;
public:
	std::vector<unsigned> itemNumber;
	void draw();
	void setItem(unsigned num, bool state);
	void setItemList(std::vector<vwItem> * list);
};

#endif
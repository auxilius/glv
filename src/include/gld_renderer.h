#include <vector>
#include <string>
#include <sstream>
#include "gld_types.h"
#include "gld_interface.h"

void setColorByValue(float value);

class View {
public:
	View(){};
	Box border;
	unsigned layer;
	void drawBackground(float r, float g, float b);
	void drawBorder(float r, float g, float b);
	void setBorder(Box newBorder);
};

struct TextureListItem {
	TextureListItem() {};
	TextureListItem(std::string C, GLuint ID) : Caption(C), textureID(ID) {};
	std::string Caption;
	GLuint textureID;
};
class TextureView : public View {
private:
	GLuint textureShown;
	GLuint texWidth, texHeight;
	float texRatio;
public:
	TextureView() :textureShown(0) {};
	TextureView(Box cBorder) :textureShown(0) { border = cBorder; };
	GLuint getTextureShown() { return textureShown; };
	void showTextureWithID(GLuint ID);
	void draw();
};

struct VariableWatchData {
	void * data;
	short param;
	char type;
	std::string getString();
};
struct VariableWatchLine {
private:
	std::vector<std::string> text;
	std::vector<VariableWatchData> value;
public:
	char * getText();
	void addData(std::string format, void* data[]);
};
class VariableView : public View {
private:
	std::vector<VariableWatchLine> *lineList;
public:
	std::vector<unsigned> lineNumber;
	void draw();
	void setLine(int num, bool state);
	void setLineList(std::vector<VariableWatchLine> * list);
};

class Model {
private:
	GLfloat scaleFactor, translateFactor[3];
	bool isFactorCalculated;
	std::string	caption;
	GLuint vboID;
	float* data;
	bool normalized;
	float minValue, maxValue;

	unsigned verticeCount;
	float normalizeValue(float value);
	void calculateTransformation();
public:
	Model() { set("", 0, NULL, 0); };
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vboID; };
	void set(std::string C, GLuint ID, float* D, unsigned N);
	void setData(float* data, float min, float max);
	void render();
};
class ModelView : public View {
private:
	Model * model;
	bool wasSelected;
	Point lastMousePos;
	double hang, vang, dist;
	float normalizeValue(float value);
public:
	ModelView() { model = NULL; };
	ModelView(Box cBorder) { border = cBorder; model = NULL; };
	void showModel(Model * M);
	void draw();
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
};


class gldRenderer {
private:
	PopupMenu popupModelSelect;
	std::vector<Model> modelList;
	std::vector<ModelView> modelField;

	PopupMenu popupTextureSelect;
	std::vector<TextureListItem> textureList;
	std::vector<TextureView> textureField;

	CheckPopupMenu popupVariableSelect;
	std::vector<VariableWatchLine> lineList;
	std::vector<VariableView> variableField;

	int selectedModelField;
	bool selectModelFieldUnderMouse();
	int selectedTextureField;
	bool selectTextureFieldUnderMouse();
	int selectedVariableField;
	bool selectVariableFieldUnderMouse();
	unsigned selectFieldUnderMouse();

	void clearConfiguration();
public:
	bool * renderTrigger;
	void requestRender() { *renderTrigger = true; }
	void init();
	void render();
	bool loadConfiguration();

	void mouseDown(mouseButton button);
	void mouseUp(mouseButton button);
	void mouseMove(int x, int y);
	void mouseWheel(signed short direction);
	
	bool addTexture(const char * caption, GLuint textureID);
	bool addValues(const char * caption, const char * formatString, void* data[]);
	bool addModel(const char * caption, GLuint vboid, unsigned count, float* data = NULL);
	void addModelData(GLuint vboid, float* data, float minValue, float maxValue);
	void addModelData(const char * caption, float* data, float minValue, float maxValue);
	
};

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
	TextureListItem() : caption(""), bufferID(0) {};
	TextureListItem(std::string C, GLuint ID) : caption(C), bufferID(ID) {};
	std::string caption;
	GLuint bufferID;
};
class TextureView : public View {
private:
	std::vector<TextureListItem> * textureList;
	int textureListIndex;
	GLuint texWidth, texHeight;
	float texRatio;
	void clearData();
public:
	std::string waitingForTextureCaption;
	TextureView() { clearData(); };
	TextureView(Box cBorder) { clearData(); border = cBorder; };
	GLuint getTextureID();
	std::string getTextureCaption();
	void showTexture(int index);
	void draw();
	void setTextureList(std::vector<TextureListItem> * list);
};

struct VariableWatchData {
private:
	bool error;
public:	
	VariableWatchData();
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
	void setLine(unsigned num, bool state);
	void setLineList(std::vector<VariableWatchLine> * list);
};

class Model {
private:
	GLfloat scaleFactor, translateFactor[3];
	bool isFactorCalculated;
	std::string	caption;
	GLuint vertexID, indexID;
	float* data;
	bool normalized;
	float minValue, maxValue;
	unsigned verticeCount;
	float normalizeValue(float value);
	void calculateTransformation();
public:
	Model();
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vertexID; };
	void set(std::string C, unsigned N, unsigned VID, unsigned IID);
	void setData(float* data, float min, float max);
	void render();
};
class ModelView : public View {
private:
	std::vector<Model> * modelList;
	int modelListIndex;
	bool wasSelected;
	Point lastMousePos;
	double hang, vang, dist;
	float normalizeValue(float value);
public:
	std::string waitingForModelCaption;
	ModelView() { modelList = NULL; };
	ModelView(Box cBorder) { border = cBorder; modelList = NULL; };
	void showModel(int index);
	std::string getModelCaption();
	Model * getModel();
	void draw();
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
	void setModelList(std::vector<Model> * list) {
		modelList = list;
	};
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

	void clearFields();
public:
	bool * renderTrigger;
	void requestRender() { *renderTrigger = true; }
	void init();
	void render();
	
	bool save();
	bool load();

	void mouseDown(mouseButton button);
	void mouseUp(mouseButton button);
	void mouseMove(int x, int y);
	void mouseWheel(signed short direction);
	
	bool addTexture(const char * caption, GLuint textureID);
	bool addValues(const char * caption, const char * formatString, void* data[]);
	bool addModel(const char * caption, const unsigned count, const GLuint vertices, const GLuint indices);
	void addModelData(GLuint vboid, float* data, float minValue, float maxValue);
	void addModelData(const char * caption, float* data, float minValue, float maxValue);
	
};

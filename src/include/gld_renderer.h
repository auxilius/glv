#include <vector>
#include <string>
#include <sstream>
#include "gld_types.h"
#include "gld_interface.h"

void setColorByValue(float value);

class View {
public:
	View(){};
	explicit View(Box cBorder) : border(cBorder) {};
	Box border;
	void drawBackground(float r, float g, float b);
	void drawBorder(float r, float g, float b);
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
	std::vector<std::string> text;
	std::vector<VariableWatchData> value;
	char * getText();
	void addData(std::string format, void* data[]);
};
class VariableWatchView : public View {
public: 
	VariableWatchView() {};
	std::vector<VariableWatchLine> line;
	void draw();
};

struct Model {
	Model(){
		caption	= "";
		vboID = 0;
		data	= NULL;
		normalized	= true;
		minValue = 0;
		maxValue = 0;
		verticeCount = 0;
	};
	Model(std::string C, GLuint ID, float* D, unsigned N) : caption(C), vboID(ID), data(D), verticeCount(N) {};
	void setDataLimits(float min, float max) { 
		minValue = min;
		maxValue = max;
		normalized = false;
	};
	std::string	caption;
	GLuint vboID;
	float* data;
	bool normalized;
	float minValue, maxValue;
	unsigned verticeCount;
};
class ModelView : public View {
private:
	Model model;
	bool wasSelected;
	Point lastMousePos;
	double hang, vang, dist;
	float normalizeValue(float value);
public:
	ModelView() { };
	ModelView(Box cBorder) { border = cBorder; };
	void showModel(Model M);
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

	VariableWatchView variableField;

	int selectedModelField;
	bool selectModelFieldUnderMouse();
	int selectedTextureField;
	bool selectTextureFieldUnderMouse();

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
	
	bool addTexture(char * caption, GLuint textureID);
	bool addValues(std::string formatString, void* data[]);
	bool addValues(std::string formatString, void* data);
	bool addModel(char * caption, GLuint vboid, unsigned count, float* data = NULL);
	void addModelData(GLuint vboid, float* data, float minValue, float maxValue);
	void addModelData(char * caption, float* data, float minValue, float maxValue);
	
};

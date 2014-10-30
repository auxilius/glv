#include <vector>
#include <string>
#include <sstream>
#include "gld_types.h"
#include "gld_interface.h"
#include "gld_visual_view.h"
#include "gld_visual_view_variable.h"

void setColorByValue(float value);

class TextureObject {
private:
	std::string caption;
	GLuint bufferID;
	GLint width, height;
	float ratio;
public:
	TextureObject() : caption(""), bufferID(0) {};
	TextureObject(std::string C, GLuint ID);
	void set(GLuint ID);
	std::string getCaption() { return caption; };
	void render(Box frame);
};
class TextureView : public View {
private:
	std::vector<TextureObject> * list;
	int index;
	TextureObject * getTexture();
public:
	std::string waitingForTextureCaption;
	TextureView() : index(-1), list(NULL) {};
	TextureView(Box cBorder) : index(-1), list(NULL) { border = cBorder; };
	void setTextureList(std::vector<TextureObject> * pointer);
	void showTexture(int id);
	std::string getTextureCaption();
	void render();
};

class ModelObject {
private:
	std::string	caption;
	struct MOFactor {
		GLfloat scale;
		GLfloat translate[3];
		bool isCalculated;
	} factor;
	struct MOVertices {
		GLuint bid;
		unsigned count;
		GLenum type;
	} vertice;
	struct MOEdges {
		GLuint bid;
		GLenum mode;
		GLenum type;
		unsigned count;
		bool show;
	} edges;
	struct MOTexture {
		GLuint coords;
		GLuint id;
		GLenum type;
		bool show;
	} texture;
	struct MOData {
		float* values;
		float minValue, maxValue;
		bool normalized;
		bool show;
	} data;
	float normalizeValue(float value);
	void calculateTransformation();
	void renderPoints();
	void renderColoredPoints();
	void renderTextured();
	void renderEdges();
public:
	ModelObject();
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vertice.bid; };
	void set(std::string C, unsigned N, unsigned VID, GLenum type);
	void setData(float* P, float min, float max);
	void setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void setTexture(const GLuint tex, const GLuint coords, GLenum type);
	void render();
};
class ModelView : public View {
private:
	std::vector<ModelObject> * modelList;
	int modelListIndex;
	bool wasSelected;
	Point lastMousePos;
	float normalizeValue(float value);
	void resetCamera();
public:
	double hang, vang, dist;
	std::string waitingForModelCaption;
	ModelView();
	ModelView(Box cBorder);
	void showModel(int index);
	std::string getModelCaption();
	ModelObject * getModel();
	void draw();
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
	void setModelList(std::vector<ModelObject> * list) {
		modelList = list;
	};
};


class gldRenderer {
private:
	PopupMenu popupModelSelect;
	std::vector<ModelObject> modelList;
	std::vector<ModelView> modelField;

	PopupMenu popupTextureSelect;
	std::vector<TextureObject> textureList;
	std::vector<TextureView> textureField;

	CheckPopupMenu popupVariableSelect;
	std::vector<vwItem> variableItemList;
	std::vector<VariableView> variableField;

	int selectedModelField;
	bool selectModelFieldUnderMouse();
	int selectedTextureField;
	bool selectTextureFieldUnderMouse();
	int selectedVariableField;
	bool selectVariableFieldUnderMouse();
	unsigned selectFieldUnderMouse();

	void clearFields();
	ModelObject * findModel(const char * caption);
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
	
	bool addValues(const char * caption, const char * formatString, void * data[]);
	bool addValArray(const char* caption, void* data[], const char* format, int length, int lineLength);
	
	bool addModel(const char * caption, const unsigned count, const GLuint vboid, GLenum type);
	void addModelEdges(const char * caption, const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void addModelTexture(const char * caption, const GLuint texture, const GLuint coordinates, GLenum type);
	void addModelData(const char * caption, float* data, float minValue, float maxValue);
	
};

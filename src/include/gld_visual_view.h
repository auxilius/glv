#ifndef H_GLD_VISUALIZER_VIEW
#define H_GLD_VISUALIZER_VIEW

#include "gld_types.h"
#include "glv_visual_model.h"
#include "glv_Interface.h"
#include "glv_Shaders.h"

class View {
public:
	View(){};
	Box border;
	unsigned layer;
	void drawBackground(Color color);
	void drawBorder(Color color);
	void setBorder(Box newBorder);
};


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



class ModelView : public View {
private:
	std::vector<ModelObject> * modelList;
	int modelListIndex;
	bool wasSelected;
	Point lastMousePos;
	float normalizeValue(float value);
	void resetCamera();
	Interface::CheckButton btnVerticesOver;
	Interface::CheckButton btnColormap;
	Interface::CheckButton btnAxes;
public:
	double hang, vang, dist;
	std::string waitingForModelCaption;
	void init();
	ModelView();
	ModelView(Box cBorder);
	void showModel(int index);
	std::string getModelCaption();
	ModelObject * getModel();
	void draw();
	void renderUI();
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
	void setModelList(std::vector<ModelObject> * list) {modelList = list;};
};



#endif
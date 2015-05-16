#ifndef H_GLD_VISUALIZER_VIEW
#define H_GLD_VISUALIZER_VIEW

#include "gld_types.h"
#include "glv_Field.h"
#include "glv_Model.h"
#include "glv_Interface.h"
#include "glv_Shaders.h"
#include "gld_visual_view.h"


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
	GLuint getID() { return bufferID; };
	void onRender(Box frame, float origX = false, float origY = false);
};

class TextureView : public View {
public:
	// constructor
	TextureView(Box cBorder, std::vector<TextureObject> * tex_list);
	// GETting all parameter to save
	FieldParams getParams();
	// SETting all parameter to load
	void setParams(FieldParams param);
	// event callbacks
	void onRender();
	void onTextureSelect(int textureID);
	void onTextureAdd(std::string caption, int textureObjectID);
	void onMouseDown(mouseButton button);

private:
	Interface::CheckButton btnFlipX, btnFlipY;
	int actualTexture;
	std::vector<TextureObject> * textureList;
	std::string waitingForTextureCaption;
	
	void initUI();
	TextureObject * getTexture();
	std::string getTextureCaption();
	int getTextureID();
};



class ModelView : public View {
public:
	ModelView(Box cBorder, std::vector<ModelObject> * list);

	FieldParams getParams();
	void setParams(FieldParams param);

	void onRender();

	void onModelSelect(int modelID);
	void onModelAdd(std::string caption, int modelID);
	void onModelPropertyAdd(std::string caption);
	
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
	
private:
	Interface::CheckButton btnColormap;
	Interface::CheckButton btnAxes;
	Interface::Select selectRenderMode;

	double hang, vang, dist;
	bool isBeingManipulated;
	Point lastMousePos;

	int actualModel;
	std::vector<ModelObject> * modelList;
	short waitingForRenderMode;
	std::string waitingForModelCaption;
	ModelObject* getModel();
	std::string getModelCaption();

	void initUI();
	void loadRenderModes();
	void resetCamera();
	void renderAxes();
	void renderUI();
};



#endif
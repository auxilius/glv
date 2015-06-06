#pragma once
#ifndef GLV_H_VIEW_TYPES
#define GLV_H_VIEW_TYPES

#include "gld_types.h"
#include "glv_Field.h"
#include "glv_Model.h"
#include "glv_Interface.h"
#include "glv_Shaders.h"

namespace glv {

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
	int channels;
public:
	TextureObject() : caption(""), bufferID(0), channels(0) {};
	TextureObject(std::string C, GLuint ID, int visible_channels);

	void set(GLuint ID, int visible_channels);
	std::string getCaption() { return caption; };
	std::string getChannelStr();
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
	std::string getLabel();
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

}; // namespace glv

#endif
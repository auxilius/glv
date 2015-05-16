#pragma once
#ifndef GLV_H_VIEW_CONTROL
#define GLV_H_VIEW_CONTROL

#include <vector>
#include <string>
#include <sstream>
#include "gld_types.h"
#include "glv_Interface.h"
#include "gld_visual_view.h"
#include "gld_visual_view_variable.h"
#include "glv_FieldManager.h"

class ViewModeControl {
private:
	FieldManager *fieldManager;

	Interface::Menu popupModelSelect;
	Interface::Menu popupTextureSelect;
	Interface::CheckMenu popupVariableSelect;
	Interface::CheckMenu popupShaderSelect;

	std::vector<ModelObject> modelList;
	std::vector<TextureObject> textureList;
	std::vector<vwItem> variableItemList;

	std::vector<ModelView> modelField;
	std::vector<TextureView> textureField;
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

	ViewModeControl(FieldManager *fManager);

	void requestRender() { *renderTrigger = true; }
	void init();
	void render();
	
	bool saveParams();
	bool loadParams();

	void mouseDown(mouseButton button);
	void mouseUp(mouseButton button);
	void mouseMove(int x, int y);
	void mouseWheel(signed short direction);
	
	bool addTexture(const char * caption, GLuint textureID);
	
	bool addValues(const char * caption, const char * formatString, void * data[]);
	bool addValArray(const char* caption, void* data[], const char* format, int length, int lineLength);
	
	bool addModel(const char * caption, const unsigned count, const GLuint vboid, bool rewrite, GLenum type);
	void addModelEdges(const char * caption, const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void addModelNormals(const char * caption, const GLuint bufferid);
	void addModelTexture(const char * caption, const GLuint texture, const GLuint coordinates, GLenum type);
	void addModelData(const char * caption, float* data, float minValue, float maxValue, int colorMap);
	void addModelColor(const char * caption, float* data, float minValue, float maxValue, int colorMap);
	void addModelColorBuffer(const char * caption, GLuint bid);
	void addModelShader(const char * caption, const GLuint shaderProgram);
	void addModelVertexAttrib(const char * caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer);

private:
	void onModelMenuSelect(int itemID, std::string itemCaption);
	void onTextureMenuSelect(int itemID, std::string itemCaption);
	void onVariableMenuClick(int itemID, std::string itemCaption, bool itemState);
};


#endif
#include "glv_ViewMode.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <gl\GLU.h>

using namespace glv;


ViewModeControl::ViewModeControl(FieldManager *fManager) {
	fieldManager = fManager;
};

void ViewModeControl::init() {
	using namespace std::placeholders;
	popupModelSelect.callOnItemClick = std::bind(&ViewModeControl::onModelMenuSelect, this, _1, _2);
	popupTextureSelect.callOnItemClick = std::bind(&ViewModeControl::onTextureMenuSelect, this, _1, _2);
	popupVariableSelect.callOnItemChange = std::bind(&ViewModeControl::onVariableMenuClick, this, _1, _2, _3);
};


///     F I E L D   S E L E C T I N G     ///

bool ViewModeControl::selectModelFieldUnderMouse() {
	int topmost = -1;
	for (unsigned i = 0; i < modelField.size(); i++) {
		if (modelField[i].border.contains(input.mouse)) {
			if (topmost == -1 || modelField[topmost].layer < modelField[i].layer)
				topmost = i;
		}
	}
	selectedModelField = topmost;
	return (topmost != -1);
};

bool ViewModeControl::selectTextureFieldUnderMouse() {
	int topmost = -1;
	for (unsigned i = 0; i < textureField.size(); i++) {
		if (textureField[i].border.contains(input.mouse))
			if (topmost == -1 || textureField[topmost].layer < textureField[i].layer)
				topmost = i;
	}
	selectedTextureField = topmost;
	return (topmost != -1);
};

bool ViewModeControl::selectVariableFieldUnderMouse() {
	int topmost = -1;
	for (unsigned i = 0; i < variableField.size(); i++) {
		if (variableField[i].border.contains(input.mouse)) {
			if (topmost == -1 || variableField[topmost].layer < variableField[i].layer)
				topmost = i;
		}
	}
	selectedVariableField = topmost;
	return (topmost != -1);
};

unsigned ViewModeControl::selectFieldUnderMouse() {
	unsigned result = GLV_FIELD_TYPE_NONE;
	unsigned layer = 0;
	if (selectModelFieldUnderMouse()) {
		result = GLV_FIELD_TYPE_MODEL;
		layer = modelField[selectedModelField].layer;
	}
	if (selectTextureFieldUnderMouse()) {
		if (textureField[selectedTextureField].layer >= layer) {
			layer = textureField[selectedTextureField].layer;
			result = GLV_FIELD_TYPE_TEXTURE;
			selectedModelField = -1;
		} else
			selectedTextureField = -1;
	}
	if (selectVariableFieldUnderMouse()) {
		if (variableField[selectedVariableField].layer >= layer) {
			layer = variableField[selectedVariableField].layer;
			result = GLV_FIELD_TYPE_VALUE;
			selectedModelField = -1;
			selectedTextureField = -1;
		}
		else
			selectedVariableField = -1;
	}
	return result;
};


///     R E N D E R I N G     ///

void ViewModeControl::render() {
	glLoadIdentity();
	// MODELS
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onRender();
	// TEXTURES
	for (unsigned i = 0; i < textureField.size(); i++)
		textureField[i].onRender();
	// VARIABLES
	for (unsigned i = 0; i < variableField.size(); i++)
		variableField[i].draw();
	// menus
	popupTextureSelect.onRender();
	popupModelSelect.onRender();
	popupVariableSelect.onRender();
};


///     C O N T R O L S     ///

void ViewModeControl::onModelMenuSelect(int itemID, std::string itemCaption) {
	
	modelField[selectedModelField].onModelSelect(itemID);
	saveParams();
	selectedModelField = -1;
	popupModelSelect.hide();
};

void ViewModeControl::onTextureMenuSelect(int itemID, std::string itemCaption) {
	textureField[selectedTextureField].onTextureSelect(itemID);
	saveParams();
	selectedTextureField = -1;
	popupTextureSelect.hide();
};

void ViewModeControl::onVariableMenuClick(int itemID, std::string itemCaption, bool itemState) {
	variableField[selectedVariableField].setItem(itemID, itemState);
	saveParams();
};


void ViewModeControl::mouseDown(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseDown(button);
	}
	for (unsigned i = 0; i < textureField.size(); i++) {
		textureField[i].onMouseDown(button);
	}

	if (!popupTextureSelect.visible)
		selectedTextureField = -1;
	if (!popupModelSelect.visible)
		selectedModelField = -1;
	if (!popupVariableSelect.visible)
		selectedVariableField = -1;

	if (button == mbRight) {
		unsigned selected = selectFieldUnderMouse();
		if (selected == GLV_FIELD_TYPE_MODEL) {
			popupModelSelect.showAtMousePosition();
			popupTextureSelect.hide();
			popupVariableSelect.hide();
		} else
			if (selected == GLV_FIELD_TYPE_TEXTURE) {
				popupModelSelect.hide();
				popupTextureSelect.showAtMousePosition();
				popupVariableSelect.hide();
			} else
				if (selected == GLV_FIELD_TYPE_VALUE) {
					popupModelSelect.hide();
					popupTextureSelect.hide();
					popupVariableSelect.setCheckedOptions(&variableField[selectedVariableField].itemNumber);
					popupVariableSelect.showAtMousePosition();
				}
	}
	if (button == mbLeft) {
		if (popupTextureSelect.visible)
			popupTextureSelect.onMouseDown(button);
		if (popupModelSelect.visible)
			popupModelSelect.onMouseDown(button);
		if (popupVariableSelect.visible)
			popupVariableSelect.onMouseDown(button);
	}
	requestRender();
};

void ViewModeControl::mouseUp(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseUp(button);
	}
	saveParams();
};

void ViewModeControl::mouseMove(int x, int y) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseMove(x, y);
	}
};

void ViewModeControl::mouseWheel(signed short direction) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseWheel(direction);
	}
	saveParams();
};


///     S A V E   &   L O A D     ///

ModelObject * ViewModeControl::findModel(const char * caption) {
	for (unsigned i = 0; i < modelList.size(); i++)
	if (modelList[i].getCaption() == caption)
		return &modelList[i];
	return NULL;
}

void ViewModeControl::clearFields() {
	modelField.clear();
	textureField.clear();
	variableField.clear();
};

bool ViewModeControl::saveParams() {
	for (unsigned i = 0; i < modelField.size(); i++) {
		FieldParams set = modelField[i].getParams();
		fieldManager->fieldSetParams(modelField[i].layer, set);
	}
	for (unsigned i = 0; i < textureField.size(); i++) {
		FieldParams set = textureField[i].getParams();
		fieldManager->fieldSetParams(textureField[i].layer, set);
	}
	for (unsigned i = 0; i < variableField.size(); i++) {
		FieldParams set;
		for (unsigned l = 0; l < variableField[i].itemNumber.size(); l++)
			set.iParam.push_back(variableField[i].itemNumber[l]);
		fieldManager->fieldSetParams(variableField[i].layer, set);
	}
	fieldManager->save();
	return true;
};

bool ViewModeControl::loadParams() {
	clearFields();
	for (unsigned i = 0; i < fieldManager->fieldCount(); i++) {
		
		const FieldParams get = fieldManager->getFieldParams(i);
		const short type = fieldManager->fieldGetType(i);
		const Box border = fieldManager->fieldGetBorder(i);
		
		if (type == GLV_FIELD_TYPE_MODEL) {
			ModelView MV(border, &modelList);
			MV.layer = i;
			MV.setParams(get);
			modelField.push_back(MV);
		} 
		else
		if (type == GLV_FIELD_TYPE_TEXTURE) {
			TextureView TV(border, &textureList);
			TV.layer = i;
			TV.setParams(get);
			textureField.push_back(TV);
		} 
		else
		if (type == GLV_FIELD_TYPE_VALUE) {
			VariableView VV;
			VV.setBorder(border);
			VV.setItemList(&variableItemList);
			VV.layer = i;

			for (unsigned l = 0; l < get.iParam.size(); l++)
				VV.setItem(get.iParam[l], true);
			variableField.push_back(VV);
		}
	}
	return true;
};


///     I N T E R F A C E     ///

bool ViewModeControl::addTexture(const char * caption, GLuint textureID, int channels) {
	for (unsigned i = 0; i < textureList.size(); i++) {
		if (textureList[i].getCaption() == caption) {
			textureList[i].set(textureID, channels);
			return false;
		}
	}
	TextureObject TE(caption, textureID, channels);
	textureList.push_back(TE);
	popupTextureSelect.addItem(caption);
	for (unsigned i = 0; i < textureField.size(); i++)
		textureField[i].onTextureAdd(caption, textureList.size()-1);
	return true;
};

bool ViewModeControl::addValues(const char * caption, const char * formatString, void * data[]) {
	vwLine line;
	line.create(formatString, data);
	vwItem item;
	item.addLine(line);
	variableItemList.push_back(item);
	popupVariableSelect.addItem(caption);
	for (unsigned i = 0; i < variableField.size(); i++)
		variableField[i].setItemList(&variableItemList);
	return true;
};

bool ViewModeControl::addValArray(const char* caption, void * data[], const char* format, int length, int lineLength) {
	
	vwItem item;
	vwLine firstLine;
	firstLine.setText(caption);
	item.addLine(firstLine);
	if (lineLength <= 0)
		lineLength = length;
	for (int i = 0; i < length; i+=lineLength ) {
		vwLine line;
		if (length < i + lineLength)
			line.printArray(data, i, length % lineLength, format);
		else
			line.printArray(data, i, lineLength, format);
		item.addLine(line);
		std::cout << line.getText() << std::endl;
	}
	variableItemList.push_back(item);
	popupVariableSelect.addItem(caption);
	for (unsigned i = 0; i < variableField.size(); i++)
		variableField[i].setItemList(&variableItemList);
	return true;
};

bool ViewModeControl::addModel(const char * caption, const unsigned count, const GLuint vboid, bool rewrite, GLenum type) {
	// if the model exist, change it's vertices
	for (unsigned i = 0; i < modelList.size(); i++) {
		if (modelList[i].getCaption() == caption) {
			modelList[i].setVertices(count, vboid, type, rewrite);
			return false;
		}
	}
	// if it does not exist, create one
	ModelObject MO(caption, count, vboid, type);
	modelList.push_back(MO);
	popupModelSelect.addItem(caption);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelAdd(caption, modelList.size()-1);
	return true;
};

void ViewModeControl::addModelEdges(const char * caption, const GLenum mode, const unsigned count, const GLuint indices, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setIndices(mode, count, indices, type);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelNormals(const char * caption, const GLuint bufferid) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setNormals(bufferid);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelTexture(const char * caption, const GLuint texture, const GLuint coordinates, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setTexture(texture, coordinates, type);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelData(const char * caption, float* data, float minValue, float maxValue, int colorMap ) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setData(data, minValue, maxValue, colorMap);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelColor(const char * caption, float* data, float minValue, float maxValue, int colorMap) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setColor(data, minValue, maxValue, colorMap);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
}

void ViewModeControl::addModelColorBuffer(const char * caption, GLuint bid) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->useColorBuffer(bid);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
}

void ViewModeControl::addModelShader(const char * caption, const GLuint shaderProgram) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setShader(shaderProgram);
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelVertexAttrib(const char * caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setVertexAttrib(atributeID, size, type, buffer);	
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};

void ViewModeControl::addModelPolyline(const char * caption, GLuint verts, const GLuint indices, unsigned count, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setPolyline(verts, indices, count, type);	
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].onModelPropertyAdd(caption);
};
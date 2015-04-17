#include "glv_ViewMode.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <gl\GLU.h>


ViewModeControl::ViewModeControl(FieldManager *fManager) {
	fieldManager = fManager;
};

void ViewModeControl::init() {
	using namespace std::placeholders;
	popupModelSelect.OnItemClick = std::bind(&ViewModeControl::onModelMenuSelect, this, _1, _2);
	popupTextureSelect.OnItemClick = std::bind(&ViewModeControl::onTextureMenuSelect, this, _1, _2);
	popupVariableSelect.OnItemChange = std::bind(&ViewModeControl::onVariableMenuClick, this, _1, _2, _3);
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
	unsigned result = FIELD_TYPE_NONE;
	unsigned layer = 0;
	if (selectModelFieldUnderMouse()) {
		result = FIELD_TYPE_MODEL;
		layer = modelField[selectedModelField].layer;
	}
	if (selectTextureFieldUnderMouse()) {
		if (textureField[selectedTextureField].layer >= layer) {
			layer = textureField[selectedTextureField].layer;
			result = FIELD_TYPE_TEXTURE;
			selectedModelField = -1;
		} else
			selectedTextureField = -1;
	}
	if (selectVariableFieldUnderMouse()) {
		if (variableField[selectedVariableField].layer >= layer) {
			layer = variableField[selectedVariableField].layer;
			result = FIELD_TYPE_VALUE;
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
		modelField[i].draw();
	// TEXTURES
	for (unsigned i = 0; i < textureField.size(); i++)
		textureField[i].render();
	// VARIABLES
	for (unsigned i = 0; i < variableField.size(); i++)
		variableField[i].draw();
	// menus
	popupTextureSelect.draw();
	popupModelSelect.draw();
	popupVariableSelect.draw();
};


///     C O N T R O L S     ///

void ViewModeControl::mouseDown(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseDown(button);
	}

	if (!popupTextureSelect.isActive())
		selectedTextureField = -1;
	if (!popupModelSelect.isActive())
		selectedModelField = -1;
	if (!popupVariableSelect.isActive())
		selectedVariableField = -1;

	if (button == mbRight) {
		unsigned selected = selectFieldUnderMouse();
		if (selected == FIELD_TYPE_MODEL) {
			popupModelSelect.show(input.mouse.x, input.mouse.y);
		} else
		if (selected == FIELD_TYPE_TEXTURE) {
			popupTextureSelect.show(input.mouse.x, input.mouse.y);
		} else
		if (selected == FIELD_TYPE_VALUE) {
			popupVariableSelect.setCheckedOptions(&variableField[selectedVariableField].itemNumber);
			popupVariableSelect.show(input.mouse.x, input.mouse.y);
		}
	}
	if (button == mbLeft) {
		if (popupTextureSelect.isActive())
			popupTextureSelect.onMouseDown(button);
		if (popupModelSelect.isActive())
			popupModelSelect.onMouseDown(button);
		if (popupVariableSelect.isActive())
			popupVariableSelect.onMouseDown(button);
	}
	requestRender();
};


void ViewModeControl::onModelMenuSelect(int itemID, std::string itemCaption) {
	modelField[selectedModelField].showModel(itemID);
	saveParams();
	selectedModelField = -1;
	popupModelSelect.hide();
};

void ViewModeControl::onTextureMenuSelect(int itemID, std::string itemCaption) {
	textureField[selectedTextureField].showTexture(itemID);
	saveParams();
	selectedTextureField = -1;
	popupTextureSelect.hide();
};

void ViewModeControl::onVariableMenuClick(int itemID, std::string itemCaption, bool itemState) {
	variableField[selectedVariableField].setItem(itemID, itemState);
	saveParams();
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
		FieldParams set;
		set.dParam.push_back(modelField[i].hang);
		set.dParam.push_back(modelField[i].vang);
		set.dParam.push_back(modelField[i].dist);
		set.strParam = modelField[i].getModelCaption();
		fieldManager->fieldSetParams(modelField[i].layer, set);
	}
	for (unsigned i = 0; i < textureField.size(); i++) {
		FieldParams set;
		set.strParam = textureField[i].getTextureCaption();
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
		
		if (type == FIELD_TYPE_MODEL) {
			ModelView MV(border);
			MV.setModelList(&modelList);
			MV.layer = i;
			if (get.strParam != "") {
				bool found = false;
				for (unsigned m = 0; m < modelList.size(); m++) {
					if (modelList[m].getCaption() == get.strParam) {
						MV.showModel(m);
						found = true;
						break;
					}
				}
				if (!found)
					MV.waitingForModelCaption = get.strParam;
			}
			if ( get.dParam.size() >= 3 ) {
				MV.hang = get.dParam[0];
				MV.vang = get.dParam[1];
				MV.dist = get.dParam[2];
			}
			modelField.push_back(MV);
		} else

		if (type == FIELD_TYPE_TEXTURE) {
			TextureView TV;
			TV.setBorder(border);
			TV.setTextureList(&textureList);
			TV.layer = i;
			bool found = false;
			for (unsigned tl = 0; tl < textureList.size(); tl++) {
				if (textureList[tl].getCaption() == get.strParam) {
					TV.showTexture(tl);
					found = true;
					break;
				}
			}
			if (!found)
				TV.waitingForTextureCaption = get.strParam;
			textureField.push_back(TV);
		} else

		if (type == FIELD_TYPE_VALUE) {
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

bool ViewModeControl::addTexture(const char * caption, GLuint textureID) {
	for (unsigned i = 0; i < textureList.size(); i++) {
		if (textureList[i].getCaption() == caption) {
			textureList[i].set(textureID);
			return false;
		}
	}
	TextureObject TE(caption, textureID);
	textureList.push_back(TE);
	popupTextureSelect.addItem(caption);
	for (unsigned i = 0; i < textureField.size(); i++) {
		textureField[i].setTextureList(&textureList);
		if (textureField[i].waitingForTextureCaption == caption)
			textureField[i].showTexture(textureList.size()-1);
	}
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

bool ViewModeControl::addModel(const char * caption, const unsigned count, const GLuint vboid, GLenum type) {
	for (unsigned i = 0; i < modelList.size(); i++) {
		if (modelList[i].getCaption() == caption) {
			modelList[i].setVertices(count, vboid, type);
			return false;
		}
	}
	ModelObject MO(caption, count, vboid, type);
	modelList.push_back(MO);
	popupModelSelect.addItem(caption);
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].setModelList(&modelList);
		if (modelField[i].waitingForModelCaption == caption)
			modelField[i].showModel(modelList.size() - 1);
	}
	return true;
};

void ViewModeControl::addModelEdges(const char * caption, const GLenum mode, const unsigned count, const GLuint indices, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setIndices(mode, count, indices, type);
};

void ViewModeControl::addModelNormals(const char * caption, const GLuint bufferid) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setNormals(bufferid);
};

void ViewModeControl::addModelTexture(const char * caption, const GLuint texture, const GLuint coordinates, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setTexture(texture, coordinates, type);
};

void ViewModeControl::addModelData(const char * caption, float* data, float minValue, float maxValue, int colorMap ) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setData(data, minValue, maxValue, colorMap);
};

void ViewModeControl::addModelColor(const char * caption, float* data, float minValue, float maxValue, int colorMap) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setColor(data, minValue, maxValue, colorMap);
}

void ViewModeControl::addModelColorBuffer(const char * caption, GLuint bid) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->useColorBuffer(bid);
}

void ViewModeControl::addModelShader(const char * caption, const GLuint shaderProgram) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setShader(shaderProgram);
};

void ViewModeControl::addModelVertexAttrib(const char * caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->addVertexAttrib(atributeID, size, type, buffer);	
};
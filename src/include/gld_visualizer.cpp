#include "gld_visualizer.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <gl\GLU.h>


void gldRenderer::init() {
	
};


///     F I E L D   S E L E C T I N G     ///

bool gldRenderer::selectModelFieldUnderMouse() {
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

bool gldRenderer::selectTextureFieldUnderMouse() {
	int topmost = -1;
	for (unsigned i = 0; i < textureField.size(); i++) {
		if (textureField[i].border.contains(input.mouse))
			if (topmost == -1 || textureField[topmost].layer < textureField[i].layer)
				topmost = i;
	}
	selectedTextureField = topmost;
	return (topmost != -1);
};

bool gldRenderer::selectVariableFieldUnderMouse() {
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

unsigned gldRenderer::selectFieldUnderMouse() {
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

void gldRenderer::render() {
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

void gldRenderer::mouseDown(mouseButton button) {
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
		if (popupModelSelect.isActive()) {
			int selectedMenuItem = popupModelSelect.selectedItemNumber();
			if (selectedMenuItem != -1) {
				modelField[selectedModelField].showModel(selectedMenuItem);
				save();
				selectedModelField = -1;
			}
		}
		else
		if (popupTextureSelect.isActive()) {
			int selectedMenuItem = popupTextureSelect.selectedItemNumber();
			if (selectedMenuItem != -1) {
				textureField[selectedTextureField].showTexture(selectedMenuItem);
				save();
				selectedTextureField = -1;
			}
		}
		else
		if (popupVariableSelect.isActive()) {
			int check = popupVariableSelect.checkedItem();
			if (check != -1) {
				variableField[selectedVariableField].setItem(check, true);
				save();
			}
			else {
				int uncheck = popupVariableSelect.uncheckedItem();
				if (uncheck != -1) {
					variableField[selectedVariableField].setItem(uncheck, false);
					save();
				}
			}
		}
	}
	requestRender();
};

void gldRenderer::mouseUp(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseUp(button);
	}
	save();
};

void gldRenderer::mouseMove(int x, int y) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseMove(x, y);
	}
};

void gldRenderer::mouseWheel(signed short direction) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseWheel(direction);
	}
	save();
};


///     S A V E   &   L O A D     ///

ModelObject * gldRenderer::findModel(const char * caption) {
	for (unsigned i = 0; i < modelList.size(); i++)
	if (modelList[i].getCaption() == caption)
		return &modelList[i];
	return NULL;
}

void gldRenderer::clearFields() {
	modelField.clear();
	textureField.clear();
	variableField.clear();
};

bool gldRenderer::save() {
	ConfigFieldRecord * cField;
	for (unsigned i = 0; i < modelField.size(); i++) {
		if (configLoader.field.size() <= modelField[i].layer) 
			return false;
		cField = &configLoader.field[modelField[i].layer];
		cField->param_d.clear();
		cField->param_d.push_back(modelField[i].hang);
		cField->param_d.push_back(modelField[i].vang);
		cField->param_d.push_back(modelField[i].dist);
		cField->param_str = modelField[i].getModelCaption();
	}
	for (unsigned i = 0; i < textureField.size(); i++) {
		if (configLoader.field.size() <= textureField[i].layer)
			return false;
		cField = &configLoader.field[textureField[i].layer];
		cField->param_str = textureField[i].getTextureCaption();
	}
	for (unsigned i = 0; i < variableField.size(); i++) {
		if (configLoader.field.size() <= variableField[i].layer)
			return false;
		cField = &configLoader.field[variableField[i].layer];
		cField->param_i.clear();
		for (unsigned l = 0; l < variableField[i].itemNumber.size(); l++)
			cField->param_i.push_back(variableField[i].itemNumber[l]);
	}
	configLoader.save();
	return true;
};

bool gldRenderer::load() {
	clearFields();
	bool found = false;
	for (unsigned i = 0; i < configLoader.field.size(); i++) {
		const ConfigFieldRecord cField = configLoader.field[i];
		if (cField.type == FIELD_TYPE_MODEL) {
			ModelView MV(cField.border);
			MV.setModelList(&modelList);
			MV.layer = i;
			if (cField.param_str != "") {
				found = false;
				for (unsigned m = 0; m < modelList.size(); m++) {
					if (modelList[m].getCaption() == cField.param_str) {
						MV.showModel(m);
						found = true;
						break;
					}
				}
				if (!found)
					MV.waitingForModelCaption = cField.param_str;
			}
			if ( cField.param_d.size() >= 3 ) {
				MV.hang = cField.param_d[0];
				MV.vang = cField.param_d[1];
				MV.dist = cField.param_d[2];
			}
			modelField.push_back(MV);
		} else
		if (cField.type == FIELD_TYPE_TEXTURE) {
			TextureView TV;
			TV.setBorder(cField.border);
			TV.setTextureList(&textureList);
			TV.layer = i;
			found = false;
			for (unsigned tl = 0; tl < textureList.size(); tl++) {
				if (textureList[tl].getCaption() == cField.param_str) {
					TV.showTexture(tl);
					found = true;
					break;
				}
			}
			if (!found)
				TV.waitingForTextureCaption = cField.param_str;
			textureField.push_back(TV);
		} else
		if (cField.type == FIELD_TYPE_VALUE) {
			VariableView VV;
			VV.setBorder(cField.border);
			VV.setItemList(&variableItemList);
			VV.layer = i;
			for (unsigned l = 0; l < cField.param_i.size(); l++)
				VV.setItem(cField.param_i[l], true);
			variableField.push_back(VV);
		}
	}
	return true;
};


///     I N T E R F A C E     ///

bool gldRenderer::addTexture(const char * caption, GLuint textureID) {
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

bool gldRenderer::addValues(const char * caption, const char * formatString, void * data[]) {
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

bool gldRenderer::addValArray(const char* caption, void * data[], const char* format, int length, int lineLength) {
	
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

bool gldRenderer::addModel(const char * caption, const unsigned count, const GLuint vboid, GLenum type) {
	for (unsigned i = 0; i < modelList.size(); i++)
	if (modelList[i].getCaption() == caption) {
		modelList[i].set(caption, count, vboid, type);
		return false;
	}
	ModelObject MO;
	MO.set(caption, count, vboid, type);
	modelList.push_back(MO);
	popupModelSelect.addItem(caption);
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].setModelList(&modelList);
		if (modelField[i].waitingForModelCaption == caption)
			modelField[i].showModel(modelList.size() - 1);
	}
	return true;
};

void gldRenderer::addModelEdges(const char * caption, const GLenum mode, const unsigned count, const GLuint indices, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setIndices(mode, count, indices, type);
};

void gldRenderer::addModelTexture(const char * caption, const GLuint texture, const GLuint coordinates, GLenum type) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setTexture(texture, coordinates, type);
};

void gldRenderer::addModelData(const char * caption, float* data, float minValue, float maxValue, int colorMap ) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setData(data, minValue, maxValue, colorMap);
};

void gldRenderer::addModelColor(const char * caption, float* data, float minValue, float maxValue, int colorMap) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setColor(data, minValue, maxValue, colorMap);
}

void gldRenderer::addModelColorBuffer(const char * caption, GLuint bid) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->useColorBuffer(bid);
}

void gldRenderer::addModelShader(const char * caption, const GLuint shaderProgram) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->setShader(shaderProgram);
};

void gldRenderer::addModelVertexAttrib(const char * caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer) {
	ModelObject * model = findModel(caption);
	if (model != NULL)
		model->addVertexAttrib(atributeID, size, type, buffer);	
};
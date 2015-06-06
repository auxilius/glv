#include "glv_ViewTypes.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "gld_drawing.h"
#include "glv_Interface.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define GLV_RENDER_MODE_VERT  "VERTICES"
#define GLV_RENDER_MODE_EDGE  "WIREFRAME"
#define GLV_RENDER_MODE_SOLID "SOLID"
#define GLV_RENDER_MODE_TEX   "TEXTURED"


using namespace glv;


///		V I E W		///

#pragma region View

void View::setBorder(Box newBorder) {
	border = newBorder;
};

void View::drawBackground(Color color) {
	drawBox(&border, color);
};

void View::drawBorder(Color color) {
	glLineWidth(1.0f);
	drawBox(&border, color, false);
};

#pragma endregion View <parent>


///     T E X T U R E   V I E W     ///

#pragma region Texture Object

TextureObject::TextureObject(std::string C, GLuint ID, int visible_channels) {
	caption = C;
	set(ID, visible_channels);

};

void TextureObject::set(GLuint ID, int visible_channels) {
	bufferID = ID;
	glBindTexture(GL_TEXTURE_2D, bufferID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	ratio = (float)width / height;
	channels = visible_channels;
};

void TextureObject::onRender(Box frame, float origX, float origY) {
	if (bufferID == 0)
		return;
	float frameRatio = (float)frame.width / frame.height;
	Box texbox = frame;
	if (ratio > frameRatio) {
		texbox.width = frame.width;
		texbox.height = (int)(frame.width / ratio);
		texbox.move(0, (frame.height - texbox.height) / 2);
	}
	else {
		texbox.height = frame.height;
		texbox.width = (int)(frame.height * ratio);
		texbox.move((frame.width - texbox.width) / 2, 0);
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bufferID);
	shader->bind(progRenderTexture);
	shader->setUniform1i("width", canvas.width);
	shader->setUniform1i("height", canvas.height);
	GLfloat borders[] = {(GLfloat)texbox.left, (GLfloat)texbox.top, (GLfloat)texbox.right, (GLfloat)texbox.bottom};
	GLfloat coords[] = {origX, 1.0f-origY, 1.0f-origX, origY};
	shader->setUniform4f("corners", borders);
	shader->setUniform4f("coords", coords);
	shader->setUniform1i("channels", channels);
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4);
	shader->unbind();
	glDisable(GL_TEXTURE_2D);
};

std::string TextureObject::getChannelStr() {
	if (channels == 0) return "RGBA";
	if (channels == 1) return "RGB";
	if (channels == 2) return "R";
	if (channels == 3) return "G";
	if (channels == 4) return "B";
	if (channels == 5) return "A";
	if (channels == 6) return "RG";
	if (channels == 7) return "BA";
	return "unknown";
};

TextureObject * TextureView::getTexture() {
	if (actualTexture > -1 && textureList != NULL && actualTexture < (int)textureList->size())
		return &(textureList->operator[](actualTexture));
	return NULL;
};

#pragma endregion Texture Object


#pragma region View::Texture


void TextureView::initUI() {
	int y = border.top + 5, x = border.left + 5; 
	btnFlipX.caption = "Flip X";
	btnFlipX.checked = false;
	btnFlipX.setPosition(x, y);
	x += btnFlipX.width + 5;

	btnFlipY.caption = "Flip Y";
	btnFlipY.checked = false;
	btnFlipY.setPosition(x, y);
	x += btnFlipY.width + 5;
};


TextureView::TextureView(Box cBorder, std::vector<TextureObject> * tex_list) { 
	actualTexture = -1;
	textureList = tex_list;
	border = cBorder;	
	initUI();
};

void TextureView::onRender() {
	drawBackground(clGray10);
	drawBorder(clGray30);
	
	TextureObject * texture = getTexture();
	float sX = (btnFlipX.checked) ? 1.0f : 0.0f;
	float sY = (btnFlipY.checked) ? 1.0f : 0.0f;
	if (texture != NULL)
		texture->onRender(border, sX, sY);

	reloadProjection();

	glColor4fv(clGray80);
	fontSmall->textOut(border.left+5, border.bottom-15, getLabel());

	if ( border.contains(input.mouse) ) {
		btnFlipX.onRender();
		btnFlipY.onRender();
	}
};

FieldParams TextureView::getParams() {
	FieldParams res;
	res.strParam = getTextureCaption();
	res.cParam.push_back(btnFlipX.checked ? 't':'f');
	res.cParam.push_back(btnFlipY.checked ? 't':'f');
	return res;
};

void TextureView::setParams(FieldParams param) {
	// string 
	if (param.strParam != "-") {
		bool found = false;
		for (unsigned i = 0; i < textureList->size(); i++) {
			if (textureList->at(i).getCaption() == param.strParam) {
				onTextureSelect(i);
				found = true;
				break;
			}
		}
		if (!found)
			waitingForTextureCaption = param.strParam;
	}
	// chars
	if (param.cParam.size() >= 2) {
		btnFlipX.checked = (param.cParam[0] == 't');
		btnFlipY.checked = (param.cParam[1] == 't');
	}
};

void TextureView::onTextureSelect(int textureID) {
	actualTexture = textureID;
	waitingForTextureCaption = "";
};

std::string TextureView::getTextureCaption() {
	TextureObject * texture = getTexture();
	if (texture != NULL)
		return texture->getCaption();
	return waitingForTextureCaption;
};

int TextureView::getTextureID() {
	TextureObject * texture = getTexture();
	if (texture != NULL)
		return (int)texture->getID();
	return -1;
};

void TextureView::onTextureAdd(std::string caption, int textureObjectID) {
	if (waitingForTextureCaption == caption) {
		actualTexture = textureObjectID;
		waitingForTextureCaption = "";
	}
};

void TextureView::onMouseDown(mouseButton button) {
	btnFlipX.onMouseDown(button);
	btnFlipY.onMouseDown(button);
};

std::string TextureView::getLabel() {
	TextureObject * texture = getTexture();
	std::string text = "";
	std::string caption = getTextureCaption();
	if (texture != NULL) {
		text = caption + " (id:" + std::to_string(getTextureID());
		std::string chan = texture->getChannelStr();
		if (chan != "RGBA")
			text += ", " + chan;
		text += ")";
	}
	else if (caption != "")
		text = "Waiting for: " + caption;
	return text;
};

#pragma endregion Texture View


///     M O D E L   V I E W     ///

#pragma region View::Model

void ModelView::initUI() {
	int y = border.top + 5, x = border.left + 5; 
	btnAxes.caption = "Axes";
	btnAxes.checked = true;
	btnAxes.setPosition(x, y);
	x += btnAxes.width + 5;
	
	btnColormap.caption = "Colormap";
	btnColormap.checked = false;
	btnColormap.setPosition(x, y);
	x += btnColormap.width + 5;

	selectRenderMode.setPosition(x, y);
	loadRenderModes();
};

ModelView::ModelView(Box cBorder, std::vector<ModelObject> * list) {
	resetCamera();
	isBeingManipulated = false;
	actualModel = -1;
	border = cBorder; 
	modelList = list;
	initUI();
};

void ModelView::resetCamera(){
	hang = + 0.85*PI;
	vang = PI / 4.0;
	dist = 5.0;
};

void ModelView::onMouseMove(int x, int y) {
	if (isBeingManipulated) {
		hang -= (double)(lastMousePos.x - x) / 100.0;
		vang += (double)(lastMousePos.y - y) / 100.0;
		lastMousePos.set(x,y);
	}
	if (vang < 0.01) { vang = 0.01; }
	if (vang > PI) vang = PI;
};

void ModelView::onMouseDown(mouseButton button) {
	if (button==mbLeft && border.contains(input.mouse)) {
		isBeingManipulated = true;
		lastMousePos = input.mouse;
		btnAxes.onMouseDown(button);
		btnColormap.onMouseDown(button);
		selectRenderMode.onMouseDown(button);
	}
};

void ModelView::onMouseUp(mouseButton button) {
	if (button == mbLeft)
		isBeingManipulated = false;
};

void ModelView::onMouseWheel(signed short direction) {
	if (border.contains(input.mouse)) {
		if (direction > 0)
			dist *= 0.8;
		else
			dist *= 1.2;
	}
};

void ModelView::onRender() {
	drawBackground(clGray10);
	drawBorder(clGray30);
	
	glViewport( border.left, canvas.height-border.top-border.height, border.width, border.height );

	ModelObject *model = getModel();

	if (model != NULL && model->getCustomProgram())
		shader->bindCustom(model->getCustomProgram());
	else
		shader->bind(progRenderModel);
	
	glm::mat4 projection = glm::perspective((float)PI/4.0f, (float)border.width / border.height, 0.01f, 100.0f );
	shader->setUniformMatrix4f("glvProjectionMatrix", glm::value_ptr(projection));

	GLfloat x = (GLfloat)(dist * sin(vang) * sin(hang));
	GLfloat y = (GLfloat)(dist * cos(vang));
	GLfloat z = (GLfloat)(-dist * sin(vang) * cos(hang));
	glm::mat4 view = glm::lookAt( glm::vec3(x,y,z), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f) );
	shader->setUniformMatrix4f("glvViewMatrix", glm::value_ptr(view));

	
	if (model != NULL) {
		glm::mat4 modelMat = model->getMatrix();
		shader->setUniformMatrix4f("glvModelMatrix", glm::value_ptr(modelMat));
		model->onRender(shader, (ModelObject::RenderMode)selectRenderMode.getSelected());	
	}
	
	// render axes if user enabled them
	if (btnAxes.checked) {
		shader->bind(progRenderModel);
		glm::mat4 matIdentity = glm::mat4(1.0f);
		shader->setUniformMatrix4f("glvProjectionMatrix", glm::value_ptr(projection));
		shader->setUniformMatrix4f("glvViewMatrix", glm::value_ptr(view));
		shader->setUniformMatrix4f("glvModelMatrix", glm::value_ptr(matIdentity));
		renderAxes();
	}

	shader->unbind();
	renderUI();
};

void ModelView::renderUI() {
	reloadProjection();
	ModelObject *myModel = getModel();

	// Render Colormap
	if (btnColormap.checked) {
		if (myModel != NULL) {
			int colormap = myModel->getColormap();
			glBegin(GL_QUADS);
			int x = border.left;
			int inc = border.width/10;
			GLcolor col0 = valToColor(0.0f, colormap);
			for ( float i = 0.0f; i < 1.0f; i+=0.1f ) {
				GLcolor col1 = valToColor(i+0.1f, colormap);
				glColor3f(col0.R, col0.G, col0.B);
				glVertex2f((GLfloat)x, (GLfloat)border.top);	
				glVertex2f((GLfloat)x, (GLfloat)border.top+3);	
				glColor3f(col1.R, col1.G, col1.B);
				glVertex2f((GLfloat)x+inc, (GLfloat)border.top+3);
				glVertex2f((GLfloat)x+inc, (GLfloat)border.top);
				col0 = col1;
				x += inc;
			}
			glEnd();
		}
	}

	// Render Inteface Elements
	if ( border.contains(input.mouse) ) {
		btnAxes.onRender();
		btnColormap.onRender();
		selectRenderMode.onRender();
	}

	// Print Model Caption
	std::string modelCaption = "";
	if (myModel != NULL)
		modelCaption = myModel->getCaption();
	else if (waitingForModelCaption != "" && waitingForModelCaption != "-")
		modelCaption = waitingForModelCaption;
	glColor4fv(clGray80);
	fontSmall->textOut(border.left+5, border.bottom-15, modelCaption);
	
};

void ModelView::renderAxes() {
	const GLfloat axeLength = 0.2f;
	glLineWidth(1.5f);
	shader->setUniform1i("glvEnableValues", 0);
	shader->setUniform1i("glvEnableTexture", 0);
	shader->setUniform1i("glvEnableLightning", 0);
	GLfloat verts[] = { 0.0f, 0.0f, 0.0f, axeLength, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, axeLength, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, axeLength };
	GLuint axes;
	glGenBuffers(1, &axes);
	glBindBuffer(GL_ARRAY_BUFFER, axes);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	shader->setAttribute("glvVertex", axes, 3);
	shader->setUniform4f("glvColor", clRed);
	glDrawArrays(GL_LINES, 0, 2);
	shader->setUniform4f("glvColor", clGreen);
	glDrawArrays(GL_LINES, 2, 2);
	shader->setUniform4f("glvColor", clBlue);
	glDrawArrays(GL_LINES, 4, 2);

	glDeleteBuffers(1, &axes);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	shader->disableAttributes();
};

FieldParams ModelView::getParams() {
	FieldParams res;
	res.dParam.push_back(hang);
	res.dParam.push_back(vang);
	res.dParam.push_back(dist);
	res.cParam.push_back(btnAxes.checked ? 't':'f');
	res.cParam.push_back(btnColormap.checked ? 't':'f');
	res.cParam.push_back('0' + (char)selectRenderMode.getSelected());
	res.strParam = getModelCaption();
	return res;
};

void ModelView::setParams(FieldParams param) {
	// string parameter
	if (param.strParam != "") {
		bool found = false;
		for (unsigned m = 0; m < modelList->size(); m++) {
			if (modelList->at(m).getCaption() == param.strParam) {
				onModelSelect(m);
				found = true;
				break;
			}
		}
		if (!found)
			waitingForModelCaption = param.strParam;
	}
	// double parameters
	if ( param.dParam.size() >= 3 ) {
		hang = param.dParam[0];
		vang = param.dParam[1];
		dist = param.dParam[2];
	}
	// char parameters
	if (param.cParam.size() >= 3) {
		btnAxes.checked = (param.cParam[0] == 't');
		btnColormap.checked = (param.cParam[1] == 't');
		waitingForRenderMode = (short)(param.cParam[2] - '0');
	} 

	loadRenderModes();
};

void ModelView::onModelAdd(std::string caption, int modelID) {
	if (waitingForModelCaption == caption) {
		actualModel = modelID;
		waitingForModelCaption = "";
		loadRenderModes();
	}
};

void ModelView::onModelSelect(int modelID) {
	actualModel = modelID;
	waitingForModelCaption = "";
	loadRenderModes();
};

void ModelView::onModelPropertyAdd(std::string caption) {
	if (getModelCaption() == caption)
		loadRenderModes();
};

ModelObject* ModelView::getModel() {
	if (actualModel != -1 && modelList != NULL && actualModel < (int)modelList->size())
		return &(modelList->at(actualModel));
	return NULL;
};

std::string ModelView::getModelCaption() {
	ModelObject *model = getModel();
	if (model != NULL)
		return model->getCaption();
	return waitingForModelCaption;
};

void ModelView::loadRenderModes() {

	if ((int)waitingForRenderMode == -1)
		waitingForRenderMode = selectRenderMode.getSelected();

	selectRenderMode.clearAllItems();

	ModelObject *model = getModel();
	if (model != NULL) {

		ModelObject::RenderMode maxMode = model->getMaximalRenderMode();
		selectRenderMode.visible = ModelObject::rmVertices < maxMode;
		selectRenderMode.addItem(GLV_RENDER_MODE_VERT);
		
		if (ModelObject::rmEdges <= maxMode) {
			selectRenderMode.addItem(GLV_RENDER_MODE_EDGE);
			selectRenderMode.addItem(GLV_RENDER_MODE_SOLID);
		}

		if (ModelObject::rmTextured <= maxMode) {
			selectRenderMode.addItem(GLV_RENDER_MODE_TEX);
		}

		if (waitingForRenderMode > -1 && maxMode >= waitingForRenderMode) {
			selectRenderMode.selectItem(waitingForRenderMode);
			waitingForRenderMode = -1;
		}

	} 
	else {
		selectRenderMode.visible = false;
	}
	 
};



#pragma endregion Model View


///     V A R I A B L E   V I E W     ///

#pragma region View::Variable

// SINGLE DATA
vwSingleData::vwSingleData() {
	error = false;
	data = NULL;
	format.param = 0;
	format.type = '\0';
};
std::string vwSingleData::getString() {
	std::stringstream result;
	if (format.param != 0) {
		result
			<< std::setprecision(format.param)
			<< std::fixed
			<< std::boolalpha;
	}
	try {
		if (data == NULL)
			result << "N/A";
		else
			if (format.type == 'i')
				result << *(int*)data;
			else
				if (format.type == 'u')
					result << *(unsigned*)data;
				else
					if (format.type == 'f')
						result << *(float*)data;
					else
						if (format.type == 'd')
							result << *(double*)data;
						else
							if (format.type == 'c')
								result << *(char*)data;
							else
								if (format.type == 'b')
									result << *(bool*)data;
								else
									if (format.type == 's')
										result << (char*)data;
									else
										result << "%" << format.type;
	}
	catch (...) {
		if (!error)
			MessageBox(0, "Cannot read memory, poited value was probably ereased.", "Memory read error", MB_OK);
		result << "err";
		error = true;
	}
	return result.str();
};

// LINE
vwFormat vwLine::readFormat(const char * formatString) {
	vwFormat result;
	unsigned i = 0;
	if (formatString[0] == '%')
		i = 1;
	if (formatString[i] >= '0' && formatString[i] <= '9') {
		result.param = (formatString[i] - '0');
		i++;
	}
	result.type = formatString[i];
	return result;
};
void vwLine::create(std::string format, void* data[]) {
	unsigned data_it = 0;
	std::string stringPart = "";
	for (std::string::size_type i = 0; i < format.size(); ++i) {
		if (format[i] == '%') {
			text.push_back(stringPart);
			stringPart = "";
			i++;
			vwSingleData V;
			while (format[i] >= '0' && format[i] <= '9') {
				V.format.param = format[i] - '0';
				i++;
			}
			V.data = data[data_it];
			data_it++;
			V.format.type = format[i];
			value.push_back(V);
		}
		else
			stringPart = stringPart + format[i];
	}
	text.push_back(stringPart);
};
void vwLine::printArray(void *data[], int startPosition, int length, const char * singleFormat) {
	unsigned data_it = 0;
	text.push_back("\t");
	vwFormat format = readFormat(singleFormat);
	for (int i = 0; i < length; i++) {
		vwSingleData SD;
		SD.data = data[i];
		SD.format = format;
		value.push_back(SD);
	}
};
char * vwLine::getArray() {
	std::string out = ": ";
	for (unsigned i = 0; i < value.size(); i++) {
		out = out + value[i].getString() + " ";
	}
	char * result = new char[out.length() + 1];
	strcpy_s(result, out.length() + 1, out.c_str());
	return result;
};
char * vwLine::getText() {
	if (text.size() > 0 && text[0] == "\t")
		return getArray();
	std::string out = "";
	for (unsigned i = 0; i < text.size(); i++) {
		out = out + text[i];
		if (i < value.size())
			out = out + value[i].getString();
	}
	char * result = new char[out.length() + 1];
	strcpy_s(result, out.length() + 1, out.c_str());
	return result;
};
void vwLine::setText(std::string lineText) {
	text.push_back(lineText);
};

// ITEM
void vwItem::addLine(vwLine newLine) {
	line.push_back(newLine);
};
int vwItem::render(int x, int y) {
	const int lineHeight = fontMain->getHeight() + 5;
	int yy = y;
	for (unsigned i = 0; i < line.size(); i++) {
		fontMain->textOut( x, yy, line[i].getText() );
		yy += lineHeight;
	}
	return yy;
};


// VIEW
void VariableView::draw() {
	glColor3f(0.8f, 0.8f, 0.8f);
	int y = border.top + 5;
	int x = border.left + 5;
	for (unsigned i = 0; i < itemNumber.size(); i++) {
		if (itemNumber[i] < itemList->size())
			y = itemList->operator[](itemNumber[i]).render(x, y);
	}
};
void VariableView::setItem(unsigned num, bool state) {
	if (std::find(itemNumber.begin(), itemNumber.end(), (unsigned)num) == itemNumber.end()) {
		if (state == true)
			itemNumber.push_back((unsigned)num);
	}
	else {
		itemNumber.erase(
			std::remove(itemNumber.begin(), itemNumber.end(), (unsigned)num),
			itemNumber.end());
	}
};
void VariableView::setItemList(std::vector<vwItem> * list) {
	itemList = list;
};

#pragma endregion Variable View
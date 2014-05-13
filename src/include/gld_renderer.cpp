#include "gld_renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <gl\GLU.h>
#include "gld_types.h"

gldRenderer * globalRenderer;

void setColorByValue(float value) {
	if (value >= 0.0f && value < 0.2f) {
		value = value / 0.2f;
		glColor3f(0.0f, value, 1.0f);
	}
	else if (value >= 0.2f && value < 0.4f) {
		value = (value - 0.2f) / 0.2f;
		glColor3f(0.0f, 1.0f, 1.0f - value);
	}
	else if (value >= 0.4f && value < 0.6f) {
		value = (value - 0.4f) / 0.2f;
		glColor3f(value, 1.0f, 0.0f);
	}
	else if (value >= 0.6f && value < 0.8f) {
		value = (value - 0.6f) / 0.2f;
		glColor3f(1.0f, 1.0f - value, 0.0f);
	}
	else if (value >= 0.8f && value <= 1.0f) {
		value = (value - 0.8f) / 0.2f;
		glColor3f(1.0f, 0.0f, value);
	}
	else if (value > 1.0f)
		glColor3f(1.0f, 1.0f, 1.0f);
	else if (value < 0.0f)
		glColor3f(0.0f, 0.0f, 0.0f);
};

void gldRenderer::init() {
	globalRenderer = this;
};
bool gldRenderer::selectModelFieldUnderMouse() {
	for (unsigned i = 0; i < modelField.size(); i++) {
		if (modelField[i].border.contains(input.mouse)) {
			selectedModelField = i;
			return true;
		}
	}
	selectedModelField = -1;
	return false;
};
bool gldRenderer::selectTextureFieldUnderMouse() {
	for (unsigned i = 0; i < textureField.size(); i++) {
		if (textureField[i].border.contains(input.mouse)) {
			selectedTextureField = i;
			return true;
		}
	}
	selectedTextureField = -1;
	return false;
};
void gldRenderer::render() {
	glLoadIdentity();
	// MODELS
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].draw();
	// TEXTURES
	for (unsigned i = 0; i < textureField.size(); i++)
		textureField[i].draw();
	// VARIABLES
	if (variableField.border.width > 0)
		variableField.draw();
	// menus
	popupTextureSelect.draw();
	popupModelSelect.draw();

};
void gldRenderer::mouseDown(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseDown(button);
	}
	if (!popupTextureSelect.isActive())
		selectedTextureField = -1;
	if (!popupModelSelect.isActive())
		selectedModelField = -1;
	if (button == mbRight) {
		if (selectModelFieldUnderMouse()) {
			popupModelSelect.show(input.mouse.x, input.mouse.y);
		} else
		if (selectTextureFieldUnderMouse()) {
			popupTextureSelect.show(input.mouse.x, input.mouse.y);
		}
	}
	if (button == mbLeft) {
		if (popupModelSelect.isActive()) {
			int selectedMenuItem = popupModelSelect.selectedItemNumber();
			if (selectedMenuItem != -1) {
				modelField[selectedModelField].showModel(modelList[selectedMenuItem]);
				selectedModelField = -1;
			}
		}
		else
		if (popupTextureSelect.isActive()) {
			int selectedMenuItem = popupTextureSelect.selectedItemNumber();
			if (selectedMenuItem != -1) {
				textureField[selectedTextureField].showTextureWithID(textureList[selectedMenuItem].textureID);
				selectedTextureField = -1;
			}
		}
	}
	requestRender();
};
void gldRenderer::mouseUp(mouseButton button) {
	for (unsigned i = 0; i < modelField.size(); i++) {
		modelField[i].onMouseUp(button);
	}
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
};
void gldRenderer::clearConfiguration() {
	modelField.clear();
	textureField.clear();
	variableField.border.set(0,0,0);
};
bool gldRenderer::loadConfiguration() {
	clearConfiguration();
	std::ifstream stream;
	stream.open(FILE_CONFIG);
	if (stream.fail())
		return false;
	unsigned x;
	stream >> x >> x >> x >> x;
	unsigned numberOfFields = 0;
	stream >> numberOfFields;
	Box fieldBorder;
	unsigned visualizationType;
	for (unsigned i = 0; i < numberOfFields; i++) {
		stream >> fieldBorder.top;
		stream >> fieldBorder.right;
		stream >> fieldBorder.bottom;
		stream >> fieldBorder.left;
		stream >> visualizationType;
		if (visualizationType == VISUALIZE_MESH) {
			ModelView MV(fieldBorder);
			modelField.push_back(MV);
		}
		if (visualizationType == VISUALIZE_TEXTURE) {
			TextureView TV(fieldBorder);
			textureField.push_back(TV);
		}
		if (visualizationType == VISUALIZE_VALUE) {
			variableField.border = fieldBorder;
		}
	}
	stream.close();
	return true;
};
bool gldRenderer::addTexture(char * caption, GLuint textureID) {
	TextureListItem TE(caption, textureID);
	textureList.push_back(TE);
	popupTextureSelect.addItem(caption);
	return true;
};
bool gldRenderer::addValues(std::string formatString, void* data[]) {
	VariableWatchLine L;
	L.addData(formatString, data);
	variableField.line.push_back(L);
	return true;
};
bool gldRenderer::addValues(std::string formatString, void* data) {
	void * a_data[] = { data };
	VariableWatchLine L;
	L.addData(formatString, a_data);
	variableField.line.push_back(L);
	return true;
};
bool gldRenderer::addModel(char * caption, GLuint vboid, unsigned count, GLfloat* data) {
	Model MO(caption, vboid, data, count);
	modelList.push_back(MO);
	popupModelSelect.addItem(caption);
	return true;
};
void gldRenderer::addModelData(GLuint vboid, float* data, float minValue, float maxValue) {
	for (unsigned i = 0; i < modelList.size(); i++) {
		if (modelList[i].vboID == vboid) {
			modelList[i].data = data;
			modelList[i].setDataLimits(minValue, maxValue);
		}
	}
};
void gldRenderer::addModelData(char * caption, float* data, float minValue, float maxValue) {
	for (unsigned i = 0; i < modelList.size(); i++) {
		if (modelList[i].caption == caption) {
			modelList[i].data = data;
			modelList[i].setDataLimits(minValue, maxValue);
		}
	}
};

///		V I E W		///
void View::drawBackground(float r, float g, float b) {
	glColor3f(r, g, b);
	drawRect(GL_QUADS, border);
};
void View::drawBorder(float r, float g, float b) {
	glLineWidth(1.0f);
	glColor3f(r, g, b);
	drawRect(GL_LINE_LOOP, border);
};

///		T E X T U R E   V I E W		///
void TextureView::showTextureWithID(GLuint ID) {
	textureShown = ID;
	if (ID = 0) return;
	glBindTexture(GL_TEXTURE_2D, textureShown);
	GLint param = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);
	texWidth = param;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &param);
	texHeight = param;
	texRatio = (float)texWidth / texHeight;
};
void TextureView::draw() {
	drawBackground(0.02f, 0.02f, 0.02f);
	drawBorder(0.3f, 0.3f, 0.3f);
	if (textureShown != 0) {
		float pomerBox = (float)border.width / border.height;
		Box texbox = border;
		if (texRatio > pomerBox) {
			texbox.width = border.width;
			texbox.height = border.width / texRatio;
			texbox.move(0, (border.height - texbox.height) / 2);
		}
		else {
			texbox.height = border.height;
			texbox.width = border.height * texRatio;
			texbox.move((border.width - texbox.width) / 2, 0);
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureShown);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(texbox.left, texbox.top);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(texbox.right, texbox.top);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(texbox.right, texbox.bottom);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(texbox.left, texbox.bottom);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
};

///		V A R I A B L E   V I E W		///
std::string VariableWatchData::getString() {
	std::stringstream result;
	if (param != 0) {
		result 
			<< std::setprecision(param)
			<< std::fixed
			<< std::boolalpha;
	}
	if (data == NULL)
		result << "";
	else 
	if (type == 'i')
		result << *(int*)data;
	else
	if (type == 'u')
		result << *(unsigned*)data;
	else
	if (type == 'f')
		result << *(float*)data;
	else
	if (type == 'd')
		result << *(double*)data;
	else
	if (type == 'c')
		result << *(char*)data;
	else
	if (type == 'b')
		result << *(bool*)data;
	else
		result << "%" << type;
	return result.str();
};
void VariableWatchLine::addData(std::string format, void* data[]) {
	unsigned data_it = 0;
	std::string stringPart = "";
	for (std::string::size_type i = 0; i < format.size(); ++i) {
		if (format[i] == '%') {
			text.push_back(stringPart);
			stringPart = "";
			i++;
			VariableWatchData V;
			while (format[i] >= '0' && format[i] <= '9') {
				V.param = format[i] - '0';
				i++;
			}
			V.data = data[data_it];
			data_it++;
			V.type = format[i];
			value.push_back(V);
		}
		else
			stringPart = stringPart + format[i];
	}
	text.push_back(stringPart);
};
char * VariableWatchLine::getText() {
	std::string out = "";
	for (unsigned i = 0; i < text.size(); i++) {
		out = out + text[i];
		if (i < value.size())
			out = out + value[i].getString();
	}
	char * result = new char[out.length() + 1];
	strcpy(result, out.c_str());
	return result;
};
void VariableWatchView::draw() {
	//drawBackground(0.05f, 0.05f, 0.05f);
	//drawBorder(0.2f, 0.2f, 0.2f);
	glColor3f(0.8f, 0.8f, 0.8f);
	setFontSize(8);
	const int spacing = getTextHeight("X") + 5;
	for (unsigned i = 0; i < line.size(); i++) {
		textPrint(border.left + 5, border.top + 5 + i*spacing, line[i].getText());
	}
};

///		M O D E L   V I E W		///
void ModelView::showModel(Model M) {
	model = M;
	hang = PI / 5.0;
	vang = PI / 4.0;
	dist = 5.0;
}
float ModelView::normalizeValue(float value) {
	return (value - model.minValue) / (model.maxValue - model.minValue);
};
void ModelView::onMouseMove(int x, int y) {
	if (wasSelected) {
		hang -= (double)(lastMousePos.x - x) / 100.0;
		vang += (double)(lastMousePos.y - y) / 100.0;
		lastMousePos.set(x,y);
	}
	if (vang < 0.01) { vang = 0.01; }
	if (vang > PI/2) vang = PI/2;
};
void ModelView::onMouseDown(mouseButton button) {
	if (button==mbLeft && border.contains(input.mouse)) {
		wasSelected = true;
		lastMousePos = input.mouse;
	}
};
void ModelView::onMouseUp(mouseButton button) {
	if (button == mbLeft)
		wasSelected = false;
};
void ModelView::onMouseWheel(signed short direction) {
	if (border.contains(input.mouse)) {
		if (direction > 0)
			dist *= 0.8;
		else
			dist *= 1.2;
	}
};
void ModelView::draw() {
	drawBackground(0.05f, 0.05f, 0.05f);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(border.left, canvas.height - border.top- border.height, border.width, border.height);
	//glOrtho(-10, 10, 10, -10, -1000, 1000);
	gluPerspective(25, (float)border.width / border.height, 0.1, 100);
	GLfloat x = dist * sin(vang) * sin(hang);
	GLfloat y = dist * cos(vang);
	GLfloat z = -dist * sin(vang) * cos(hang);
	gluLookAt(x,y,z, 0,0,0, 0,1,0);

	glMatrixMode(GL_MODELVIEW);
	if (model.vboID != 0) {
		glLineWidth(1.0f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, model.vboID);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.1f, 0.1f, 0.1f);
		glDrawArrays(GL_QUADS, 0, model.verticeCount);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glPointSize(6.0f);
		glEnable(GL_POINT_SMOOTH);
		for (unsigned i = 0; i < model.verticeCount; i++) {
			if (model.data == NULL) 
				glColor3f(0.9f, 0.9f, 0.9f);
			else {
				float value = model.data[i];
				if (!model.normalized)
					value = normalizeValue(value);
				setColorByValue(value);
			}
			glDrawArrays(GL_POINTS, i, 1);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	reloadProjection();
};

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
bool gldRenderer::selectVariableFieldUnderMouse() {
	for (unsigned i = 0; i < variableField.size(); i++) {
		if (variableField[i].border.contains(input.mouse)) {
			selectedVariableField = i;
			return true;
		}
	}
	selectedVariableField = -1;
	return false;
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
void gldRenderer::render() {
	glLoadIdentity();
	// MODELS
	for (unsigned i = 0; i < modelField.size(); i++)
		modelField[i].draw();
	// TEXTURES
	for (unsigned i = 0; i < textureField.size(); i++)
		textureField[i].draw();
	// VARIABLES
	for (unsigned i = 0; i < variableField.size(); i++)
		variableField[i].draw();
	// menus
	popupTextureSelect.draw();
	popupModelSelect.draw();
	popupVariableSelect.draw();
};
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
			popupVariableSelect.setCheckedOptions(&variableField[selectedVariableField].lineNumber);
			popupVariableSelect.show(input.mouse.x, input.mouse.y);
		}
	}
	if (button == mbLeft) {
		if (popupModelSelect.isActive()) {
			int selectedMenuItem = popupModelSelect.selectedItemNumber();
			if (selectedMenuItem != -1) {
				modelField[selectedModelField].showModel(&modelList[selectedMenuItem]);
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
		else
		if (popupVariableSelect.isActive()) {
			int check = popupVariableSelect.checkedItem();
			if (check != -1)
				variableField[selectedVariableField].setLine(check, true);
			else {
				int uncheck = popupVariableSelect.uncheckedItem();
				if (uncheck != -1)
					variableField[selectedVariableField].setLine(uncheck, false);
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
	variableField.clear();
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
		if (visualizationType == FIELD_TYPE_MODEL) {
			ModelView MV;
			MV.setBorder(fieldBorder);
			MV.layer = i;
			modelField.push_back(MV);
		}
		if (visualizationType == FIELD_TYPE_TEXTURE) {
			TextureView TV;
			TV.setBorder(fieldBorder);
			TV.layer = i;
			textureField.push_back(TV);
		}
		if (visualizationType == FIELD_TYPE_VALUE) {
			VariableView VV;
			VV.setBorder(fieldBorder);
			VV.setLineList(&lineList);
			VV.layer = i;
			variableField.push_back(VV);			
		}
	}
	stream.close();
	return true;
};
bool gldRenderer::addTexture(const char * caption, GLuint textureID) {
	TextureListItem TE(caption, textureID);
	textureList.push_back(TE);
	popupTextureSelect.addItem(caption);
	return true;
};
bool gldRenderer::addValues(const char * caption, const char * formatString, void* data[]) {
	VariableWatchLine Line;
	Line.addData(formatString, data);
	lineList.push_back(Line);
	popupVariableSelect.addItem(caption);
	return true;
};
bool gldRenderer::addModel(const char * caption, GLuint vboid, unsigned count, GLfloat* data) {
	Model MO;
	MO.set(caption, vboid, data, count);
	modelList.push_back(MO);
	popupModelSelect.addItem(caption);
	return true;
};
void gldRenderer::addModelData(GLuint vboid, float* data, float minValue, float maxValue) {
	
	for (unsigned i = 0; i < modelList.size(); i++)
		if (modelList[i].getVBO() == vboid) {
			modelList[i].setData(data, minValue, maxValue);
			return;
		}
};
void gldRenderer::addModelData(const char * caption, float* data, float minValue, float maxValue) {
	for (unsigned i = 0; i < modelList.size(); i++)
		if (modelList[i].getCaption() == caption) {
			modelList[i].setData(data, minValue, maxValue);
			return;
		}
};

///		V I E W		///
void View::setBorder(Box newBorder) {
	border = newBorder;
};
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
	drawBackground(0.04f, 0.04f, 0.04f);
	drawBorder(0.1f, 0.1f, 0.1f);
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
		glTexCoord2f(0.0f, 1.0f); glVertex2f(texbox.left-1, texbox.top-1);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(texbox.right, texbox.top-1);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(texbox.right, texbox.bottom);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(texbox.left-1, texbox.bottom);
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
		result << "N/A";
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
	if (type == 's')
		result << (char*)data;
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
void VariableView::draw() {
	//drawBackground(0.05f, 0.05f, 0.05f);
	//drawBorder(0.2f, 0.2f, 0.2f);
	glColor3f(0.8f, 0.8f, 0.8f);
	setFontSize(8);
	const int spacing = getTextHeight("X") + 5;
	for (unsigned i = 0; i < lineNumber.size(); i++) {
		textPrint(border.left + 5, border.top + 5 + i*spacing,  
			lineList->operator[](lineNumber[i]).getText() );
	}
};
void VariableView::setLine(int num, bool state) {
	if (state == true && std::find(lineNumber.begin(), lineNumber.end(), (unsigned)num) == lineNumber.end()) {
		lineNumber.push_back((unsigned)num);
	}
	else if (std::find(lineNumber.begin(), lineNumber.end(), (unsigned)num) != lineNumber.end()) {
		lineNumber.erase(
			std::remove(lineNumber.begin(), lineNumber.end(), (unsigned)num), 
			lineNumber.end());
	}
};
void VariableView::setLineList(std::vector<VariableWatchLine> * list) {
	lineList = list; 
};

///		M O D E L   V I E W		///
void Model::set(std::string C, GLuint ID, float* D, unsigned N) {
	caption = C;
	vboID = ID;
	data = D;
	verticeCount = N;
	normalized = true;
	minValue = 0;
	maxValue = 0;
	calculateTransformation();
};
void Model::setData(float* data, float min, float max) {
	this->data = data;
	minValue = min;
	maxValue = max;
	normalized = (min == 0.0f) && (max == 1.0f);

	std::cout << "Data: ";
	for (unsigned i = 0; i < verticeCount; i++) {
		std::cout << data[i] << " ";
	}
	std::cout << std::endl;
};
void Model::calculateTransformation() {
	if (vboID == 0) 
		return;
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	GLfloat* vert = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLfloat maxpos[3], minpos[3];
	memcpy(maxpos, vert, 3 * sizeof(GLfloat));
	memcpy(minpos, vert, 3*sizeof(GLfloat));
	for (unsigned v = 1; v < verticeCount; v++) {
		GLfloat position[3];// = *(vert + ((v * 3) + c));
		memcpy(position, (vert + 3 * v), 3 * sizeof(GLfloat));
		for (unsigned c = 0; c < 3; c++) {
			if (position[c] < minpos[c])
				minpos[c] = position[c];
			if (position[c] > maxpos[c])
				maxpos[c] = position[c];
		}
	}
	GLfloat delta[3], maxDelta = 0;
	for (unsigned i = 0; i < 3; i++) {
		delta[i] = maxpos[i] - minpos[i];
		if (delta[i] > maxDelta) 
			maxDelta = delta[i];
	}
	scaleFactor = 1.0f / maxDelta;
	std::cout << "scaleFactor = " << scaleFactor << std::endl;
	std::cout << "translateFactor = ";
	for (unsigned i = 0; i < 3; i++) {
		translateFactor[i] = -minpos[i] - delta[i] / 2;
		std::cout << translateFactor[i] << "   " << std::endl;
	}
};
float Model::normalizeValue(float value) {
	return (value - minValue) / (maxValue - minValue);
};
void Model::render() {
	if (vboID == 0)
		return;
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);
	glTranslatef(translateFactor[0], translateFactor[1], translateFactor[2]);
	glLineWidth(1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.1f, 0.1f, 0.1f);
	glDrawArrays(GL_QUADS, 0, verticeCount);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(6.0f);
	glEnable(GL_POINT_SMOOTH);
	for (unsigned i = 0; i < verticeCount; i++) {
		if (data == NULL)
			glColor3f(0.9f, 0.9f, 0.9f);
		else {
			float value = data[i];
			if (!normalized)
				value = normalizeValue(value);
			setColorByValue(value);
		}
		glDrawArrays(GL_POINTS, i, 1);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
};

void ModelView::showModel(Model * M) {
	model = M;
	hang = PI / 5.0;
	vang = PI / 4.0;
	dist = 5.0;
}
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
	if (model != NULL)
		model->render();
	reloadProjection();
};

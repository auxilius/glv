#include "gld_visual_view.h"

#include "gld_drawing.h"
#include "gld_shaders.h"



///		V I E W		///

#pragma region View

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

#pragma endregion View <parent>



///		T E X T U R E   V I E W		///

#pragma region View::Texture

TextureObject::TextureObject(std::string C, GLuint ID) {
	caption = C;
	set(ID);
};

void TextureObject::set(GLuint ID) {
	bufferID = ID;
	glBindTexture(GL_TEXTURE_2D, bufferID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	ratio = (float)width / height;
};

void TextureObject::render(Box frame) {
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
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f((GLfloat)texbox.left - 1.0f, (GLfloat)texbox.top - 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f((GLfloat)texbox.right, (GLfloat)texbox.top - 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f((GLfloat)texbox.right, (GLfloat)texbox.bottom);
	glTexCoord2f(0.0f, 0.0f); glVertex2f((GLfloat)texbox.left - 1.0f, (GLfloat)texbox.bottom);
	glEnd();
	glDisable(GL_TEXTURE_2D);
};

TextureObject * TextureView::getTexture() {
	if (index != -1 && list != NULL && index < (int)list->size())
		return &(list->operator[](index));
	return NULL;
};

std::string TextureView::getTextureCaption() {
	TextureObject * texture = getTexture();
	if (texture != NULL)
		return texture->getCaption();
	return "";
};

void TextureView::showTexture(int id) {
	index = id;
	waitingForTextureCaption = "";
};

void TextureView::setTextureList(std::vector<TextureObject> * pointer) {
	list = pointer;
};

void TextureView::render() {
	drawBackground(0.04f, 0.04f, 0.04f);
	drawBorder(0.1f, 0.1f, 0.1f);
	TextureObject * texture = getTexture();
	if (texture != NULL)
		return texture->render(border);
};

#pragma endregion Texture View


///		MODEL VIEW		///

#pragma region View::Model

ModelView::ModelView() { 
	wasSelected = false;
	modelList = NULL; 
	modelListIndex = -1; 
	resetCamera();
};

ModelView::ModelView(Box cBorder) { 
	border = cBorder; 
	wasSelected = false;
	modelList = NULL; 
	modelListIndex = -1; 
	resetCamera();
};

void ModelView::resetCamera(){
	hang = + 0.85*PI;
	vang = PI / 4.0;
	dist = 5.0;
};

ModelObject * ModelView::getModel() {
	if (modelListIndex != -1 && modelList != NULL && modelListIndex < (int)modelList->size())
		return &(modelList->operator[](modelListIndex));
	return NULL;
};

void ModelView::showModel(int index) {
	modelListIndex = index;
	waitingForModelCaption = "";
	wasSelected = false;
}

std::string ModelView::getModelCaption() {
	ModelObject * model = getModel();
	if (model != NULL)
		return model->getCaption();
	return "";
};

void ModelView::onMouseMove(int x, int y) {
	if (wasSelected) {
		hang -= (double)(lastMousePos.x - x) / 100.0;
		vang += (double)(lastMousePos.y - y) / 100.0;
		lastMousePos.set(x,y);
	}
	if (vang < 0.01) { vang = 0.01; }
	if (vang > PI) vang = PI;
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
	gluPerspective(25, (float)border.width / border.height, 0.1, 100);

	GLfloat x = (GLfloat)(dist * sin(vang) * sin(hang));
	GLfloat y = (GLfloat)(dist * cos(vang));
	GLfloat z = (GLfloat)(-dist * sin(vang) * cos(hang));
	gluLookAt(x,y,z, 0,0,0, 0,1,0);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);

	glLineWidth(1.0f);
	GLfloat axeLength = 0.25f;
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(axeLength, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, axeLength, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, axeLength);
	glEnd();

	ModelObject * model = getModel();
	if (model != NULL)
		model->render();

	glDisable(GL_DEPTH_TEST);

	reloadProjection();
	// render GUI
};

#pragma endregion Model View
#include "gld_visual_view.h"

#include "gld_drawing.h"
#include "glv_Interface.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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
	drawBackground(clGray10);
	drawBorder(clGray30);
	TextureObject * texture = getTexture();
	if (texture != NULL)
		return texture->render(border);
};

#pragma endregion Texture View


///		MODEL VIEW		///

#pragma region View::Model

void ModelView::init() {
	wasSelected = false;
	modelList = NULL; 
	modelListIndex = -1; 
	resetCamera();

	int y = border.top + 8, x = border.left + 5; 
	btnAxes.caption = "Axes";
	btnAxes.checked = true;
	btnAxes.setPosition(x, y);
	x += btnAxes.width + 5;
	/*btnVerticesOver.caption = "Vertices";
	btnVerticesOver.setPosition(x, y);
	x += btnVerticesOver.width + 5;
	*/
	btnColormap.caption = "Colormap";
	btnAxes.checked = false;
	btnColormap.setPosition(x, y);
	x += btnColormap.width + 5;
};

ModelView::ModelView() { 
	init();
};

ModelView::ModelView(Box cBorder) { 
	border = cBorder; 
	init();
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
		btnAxes.onMouseDown(button);
		btnColormap.onMouseDown(button);
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
	drawBackground(clGray10);
	drawBorder(clGray30);
	
	// SHADER RENDER TEST
	glViewport( border.left, canvas.height-border.top-border.height, border.width, border.height );
	shader->bind(progRenderModel);
	/*
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(border.left, canvas.height - border.top- border.height, border.width, border.height);
	gluPerspective(25, , 0.1, 100); /**/
	glm::mat4 projection = glm::perspective(45.0f, (float)border.width / border.height, 0.01f, 100.0f );
	shader->setUniformMatrix4f("glvProjectionMatrix", glm::value_ptr(projection));

	GLfloat x = (GLfloat)(dist * sin(vang) * sin(hang));
	GLfloat y = (GLfloat)(dist * cos(vang));
	GLfloat z = (GLfloat)(-dist * sin(vang) * cos(hang));
	
	//gluLookAt(x,y,z, 0,0,0, 0,1,0);
	glm::mat4 view = glm::lookAt( glm::vec3(x,y,z), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f) );
	shader->setUniformMatrix4f("glvViewMatrix", glm::value_ptr(view));

	//glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	ModelObject * model = getModel();

	if (model != NULL) {

		glm::mat4 modelMat = model->getMatrix();
		shader->setUniformMatrix4f("glvModelMatrix", glm::value_ptr(modelMat));
		model->setAttributes(shader);
		model->render(shader);	
	}
	glDisable(GL_DEPTH_TEST);
	
	/*
	if (btnAxes.checked) {
		glLineWidth(1.0f);
		GLfloat axeLength = 0.25f;
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(axeLength, 0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, axeLength, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, axeLength);
		glEnd();
	}/**/
	shader->unbind();
	reloadProjection();
	renderUI();
};

void ModelView::renderUI() {

	if (btnColormap.checked) {
		ModelObject * model = getModel();
		if (model != NULL) {
			int colormap = model->getColormap();
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

	if ( border.contains(input.mouse) ) {
		btnAxes.draw();
		btnColormap.draw();
	}
	
};

#pragma endregion Model View
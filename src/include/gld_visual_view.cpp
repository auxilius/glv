#include "gld_visual_view.h"

#include "gld_drawing.h"
#include "gld_shaders.h"


void valToColor_Rainbow(float value) {
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

void valToColor_BlueRed(float value) {
	glColor3f( 1.0f - value, 0.0f, value );
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




///		M O D E L   V I E W		///

ModelObject::ModelObject() {
	set("", 0, 0, GL_FLOAT); 
	data.show = false;
	edges.show = false;
	texture.show = false;
	shader.show = false;
};

void ModelObject::set(std::string C, unsigned N, unsigned VID, GLenum type) {
	caption = C;
	vertice.bid = VID;
	vertice.count = N;
	vertice.type = type;
	calculateTransformation();
};

void ModelObject::setData(float* P, float min, float max, int cl_map) {
	data.values = P;
	data.minValue = min;
	data.maxValue = max;
	data.normalized = (min == 0.0f) && (max == 1.0f);
	data.show = (P != NULL);
	data.colormap = cl_map;
};

void ModelObject::setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type) {
	edges.bid = indices;
	edges.count = count;
	edges.mode = mode;
	edges.type = type;
	edges.show = (count != 0) && (indices != 0);
};

void ModelObject::setTexture(const GLuint tex, const GLuint coords, GLenum type) {
	texture.id = tex;
	texture.coords = coords;
	texture.type = type;
	texture.show = (tex != 0) && (coords != 0);
};

void ModelObject::setShader(const GLuint shaderProgramId) {
	shader.programId = shaderProgramId;
	shader.show = true;
};

void ModelObject::setShader(std::string shaderFile, GLenum shaderType) {
	if (!shader.show)
		shader.programId = glCreateProgram();
	shaderAddToProg(shader.programId, shaderFile, shaderType);
	shader.show = true;
};

void ModelObject::calculateTransformation() {
	if (vertice.bid == 0) 
		return;
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	GLfloat* vert = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	GLfloat maxpos[3], minpos[3];
	memcpy(maxpos, vert, 3 * sizeof(GLfloat));
	memcpy(minpos, vert, 3*sizeof(GLfloat));
	for (unsigned v = 1; v < vertice.count; v++) {
		GLfloat position[3];
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
	factor.scale = 1.0f / maxDelta;
	for (unsigned i = 0; i < 3; i++) {
		factor.translate[i] = -minpos[i] - delta[i] / 2;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
};

float ModelObject::normalizeValue(float value) {
	return (value - data.minValue) / (data.maxValue - data.minValue);
};

void ModelObject::renderPoints() {
	glPointSize(4.0f);
	glColor3f(0.9f, 0.9f, 0.9f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);
	glDrawArrays(GL_POINTS, 0, vertice.count);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
};

void ModelObject::renderColoredPoints() {
	if (!data.show)
		return;
	glPointSize(6.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);
	glEnable(GL_POINT_SMOOTH);
	for (unsigned i = 0; i < vertice.count; i++) {
		float value = data.values[i];
		if (!data.normalized)
			value = normalizeValue(value);
		if (data.colormap == COLOR_MAP_BLUERED)
			valToColor_BlueRed(value);
		else
			valToColor_Rainbow(value);
		
		glDrawArrays(GL_POINTS, i, 1);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
};

void ModelObject::renderTextured() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, texture.coords);
	glTexCoordPointer(2, texture.type, 0, NULL);

	glBindTexture(GL_TEXTURE_2D, texture.id);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edges.bid);
	glDrawElements(edges.mode, edges.count, edges.type, 0);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
};

void ModelObject::renderEdges() {
	glLineWidth(1.0f);
	glColor3f(0.1f, 0.1f, 0.1f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edges.bid);
	glDrawElements(edges.mode, edges.count, edges.type, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
};

void ModelObject::render() 
{
	if (vertice.bid == 0)
		return;
	
	glPushMatrix();
	glScalef(factor.scale, factor.scale, factor.scale);
	glTranslatef(factor.translate[0], factor.translate[1], factor.translate[2]);
	
	if (shader.show) 
		glUseProgram(shader.programId);
	
	if (edges.show) {
		if (texture.show)
			renderTextured();
		else
			renderEdges();
	} else 
		renderPoints();
	
	if (data.show)
		renderColoredPoints();
	
	glUseProgram(0);
	
	glPopMatrix();
};

ModelView::ModelView() { 
	modelList = NULL; 
	modelListIndex = -1; 
	resetCamera();
};

ModelView::ModelView(Box cBorder) { 
	border = cBorder; 
	modelList = NULL; 
	modelListIndex = -1;
	resetCamera();
};

void ModelView::resetCamera(){
	hang = PI / 5.0;
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
	ModelObject * model = getModel();
	if (model != NULL)
		model->render();
	reloadProjection();
};

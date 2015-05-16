#include "glv_Model.h"
#include "gld_drawing.h"
#include <glm/gtc/matrix_transform.hpp>



float normalize(float value, float min, float max) {
	return (value - min) / (max - min);
};


ModelObject::ModelObject(std::string capt, unsigned vertCount, GLuint verts, GLenum dataType) {
	caption = capt;
	data.exist = false;
	data.useBuffer = false;
	data.colormap = 0;
	edges.exist = false;
	texture.exist = false;
	shader.exist = false;
	setVertices(vertCount, verts, dataType, true);
	calculateMatrix(); // may crash here when VID is non existing
};




void ModelObject::calculateMatrix() {
	if (vertice.bid == 0 || vertice.count <= 0) 
		return;
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	GLfloat* vert = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	glFinish();
	// find the bounding box
	GLfloat maxpos[3], minpos[3];
	memcpy(maxpos, vert, 3 * sizeof(GLfloat));
	memcpy(minpos, vert, 3 * sizeof(GLfloat));
	for (unsigned v = 1; v < vertice.count; v++) {
		GLfloat position[3];
		memcpy(position, (vert + 3*v), 3 * sizeof(GLfloat));
		for (unsigned c = 0; c < 3; c++) {
			if (position[c] < minpos[c])
				minpos[c] = position[c];
			if (position[c] > maxpos[c])
				maxpos[c] = position[c];
		}
	}
	// unmap the buffer, we dont need it anymore
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// find largest dimension 
	GLfloat delta[3], maxDelta = 0;
	for (unsigned i = 0; i < 3; i++) {
		delta[i] = maxpos[i] - minpos[i];
		if (delta[i] > maxDelta) 
			maxDelta = delta[i];
	}
	// load the identity
	matrix = glm::mat4(1.0f);
	// scale the model so that the largest dimension is 1.0
	float scale = 1.0f / maxDelta;
	matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));
	// calculate the translation to center the object
	float trans[3];
	for (unsigned i = 0; i < 3; i++)
		trans[i] = - minpos[i] - delta[i] * 0.5f;
	matrix = glm::translate(matrix, glm::vec3(trans[0], trans[1], trans[2]));
};


void ModelObject::onRender(Shaders *shader, RenderMode mode) {
	
	glEnable(GL_DEPTH_TEST);

	// Attribute arrays
	for (unsigned i=0; i<attrib.size(); i++)
		shader->setAttribute(attrib[i].location, attrib[i].buffer, attrib[i].size, attrib[i].type);
	// inVertices
	shader->setAttribute("glvVertex", vertice.bid, 3);
	// lightning
	shader->setUniform1i("glvEnableLightning", mode==rmVertices || mode==rmEdges || mode==rmSolid);
	// texturing
	shader->setUniform1i("glvEnableTexture", mode==rmTextured && texture.exist);
	if (texture.exist)
		shader->setAttribute("glvCoords", texture.coords , 2, texture.type);
	// value -> color 
	shader->setUniform1i("glvEnableValues", (int)data.useBuffer);
	if (data.useBuffer)
		shader->setAttribute("glvValueColor", data.color_bid, 3);
	// default color
	shader->setUniform4f("glvColor", clWhite);

	// rendering cases
	if (mode == rmVertices) {
		glPointSize(5.0f);
		glEnable( GL_POINT_SMOOTH );
		glDrawArrays( GL_POINTS, 0, vertice.count);
	}
	else
	if (mode == rmEdges && edges.exist) {
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawElements();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	if (mode == rmSolid && edges.exist) {
		drawElements();
	}
	else
	if (mode == rmTextured && edges.exist && texture.exist) {
		shader->setUniform1i("glvEnableValues", 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		drawElements();
		glDisable(GL_TEXTURE_2D);
	}

	glDisable(GL_DEPTH_TEST);
};

void ModelObject::drawElements() {
	if (edges.exist) {
		glEnableClientState( GL_ELEMENT_ARRAY_BUFFER );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, edges.bid );
		glDrawElements( edges.mode, edges.count, edges.type, NULL);
		glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	}
};

void ModelObject::onRenderNormals(Shaders *shader) {
	shader->setAttribute("glvVertex", vertice.bid, 3);
	shader->setAttribute("glvNormal", normals.bid, 3);
	glLineWidth(1.0f);
	glDrawArrays( GL_POINTS, 0, vertice.count);
};


unsigned ModelObject::getColormap() {
	return data.colormap;	
};


void ModelObject::setVertices(unsigned count, GLuint verts, GLenum dataType, bool rewrite) {
	vertice.bid = verts;
	vertice.type = dataType;

	if (vertice.count != count || rewrite) {
		vertice.count = count;
		data.exist = false;
		data.useBuffer = false;
		data.colormap = 0;
		edges.exist = false;
		texture.exist = false;
		shader.exist = false;
		calculateMatrix();
	}
};

void ModelObject::setData(float* P, float min, float max, int cl_map) {
	data.values = P;
	data.minValue = min;
	data.maxValue = max;
	data.normalized = (min == 0.0f) && (max == 1.0f);
	data.exist = (P != NULL);
	data.colormap = cl_map;
};

void ModelObject::setColor(float* P, float min, float max, int cl_map) {
	if (data.useBuffer) 
		glDeleteBuffers(1, &data.color_bid);
	data.maxValue = max;
	data.minValue = min;
	data.colormap = cl_map;
	GLfloat * colors = new float[vertice.count * 3];
	for (unsigned i = 0; i < vertice.count; i++) {
		float nval = normalize(P[i], data.minValue, data.maxValue);
		GLcolor col;
		if (cl_map == COLOR_MAP_BLUERED) 
			col = valToColor_BlueRed(nval);
		else
			col = valToColor_Rainbow(nval);
		colors[3*i] = col.R;
		colors[3*i+1] = col.G;
		colors[3*i+2] = col.B;
	}
	GLuint buff = 0;
	glGenBuffersARB(1, &buff);
	glBindBufferARB(GL_ARRAY_BUFFER, buff);
	glBufferDataARB(GL_ARRAY_BUFFER, vertice.count*3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	useColorBuffer(buff);
};

void ModelObject::useColorBuffer(GLuint bid) {
	data.useBuffer = (bid != 0);
	data.color_bid = bid;
};

void ModelObject::setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type) {
	edges.bid = indices;
	edges.count = count;
	edges.mode = mode;
	edges.type = type;
	edges.exist = (count != 0) && (indices != 0);
};

void ModelObject::setTexture(const GLuint tex, const GLuint coords, GLenum type) {
	texture.id = tex;
	texture.coords = coords;
	texture.type = type;
	texture.exist = (tex != 0) && (coords != 0);
};

void ModelObject::setNormals(const GLuint bufferid) {
	normals.bid = bufferid;
	normals.exist = true;
};

void ModelObject::setShader(const GLuint shaderProgramId) {
	shader.programId = shaderProgramId;
	shader.exist = true;
};

void ModelObject::setVertexAttrib(GLuint location, GLint size, GLenum type, GLuint buffer) {
	MOAttrib newAttrib;
	newAttrib.buffer = buffer;
	newAttrib.location = location;
	newAttrib.size = size;
	newAttrib.type = type;
	attrib.push_back(newAttrib);
};

ModelObject::RenderMode ModelObject::getMaximalRenderMode() {
	if (edges.exist) {
		if (texture.exist) {
			return rmTextured;
		}
		return rmSolid;
	}
	return rmVertices;
};

GLuint ModelObject::getCustomProgram() {
	if (shader.exist)
		return shader.programId;
	return 0;
};


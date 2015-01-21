#include "glv_visual_model.h"
#include "gld_drawing.h"
#include "gld_shaders.h"



#pragma region Model Object


GLchar* default_vs = "\
void main(void)\
{\
  gl_FrontColor = gl_Color;\
  gl_BackColor = gl_Color;\
  vec4 v = gl_Vertex;\
  gl_Position = gl_ModelViewProjectionMatrix * v;\
}";
GLchar* default_fs = "void main(void) { gl_FragColor = vec4(0.4,0.4,1.0,1.0); }";


ModelObject::ModelObject() {
	set("", 0, 0, GL_FLOAT); 
	data.show = false;
	data.useBuffer = false;
	data.colormap = 0;
	edges.show = false;
	texture.show = false;
	shader.show = false;
	//createDefaultShader();
};

bool ModelObject::createDefaultShader() {

	defaultShaderProgram = glCreateProgram();

    if (defaultShaderProgram == 0) {
        fprintf(stderr, "Error creating default shader program\n");
        return false;
    }
    
	shaderAddToProg(defaultShaderProgram, default_vs, GL_VERTEX_SHADER);
	shaderAddToProg(defaultShaderProgram, default_fs, GL_FRAGMENT_SHADER);
    
	GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(defaultShaderProgram);
    glGetProgramiv(defaultShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(defaultShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
	}

    glValidateProgram(defaultShaderProgram);
    glGetProgramiv(defaultShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(defaultShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }
	return true;
}

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

void ModelObject::setColor(float* P, float min, float max, int cl_map) {
	if (data.useBuffer) 
		glDeleteBuffers(1, &data.color_bid);
	data.maxValue = max;
	data.minValue = min;
	data.colormap = cl_map;
	GLfloat * colors = new float[vertice.count * 3];
	for (unsigned i = 0; i < vertice.count; i++) {
		float nval = normalizeValue(P[i]);
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

void ModelObject::addVertexAttrib(GLuint location, GLint size, GLenum type, GLuint buffer) {
	MOAttrib newAttrib;
	newAttrib.buffer = buffer;
	newAttrib.location = location;
	newAttrib.size = size;
	newAttrib.type = type;
	attrib.push_back(newAttrib);
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
	glEnable(GL_POINT_SMOOTH);
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
	glPointSize(6.0f);
	glEnable(GL_POINT_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);
	if (data.useBuffer) {
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, data.color_bid);
		glColorPointer(3, GL_FLOAT, 0, NULL);
		glDrawArrays(GL_POINTS, 0, vertice.count);
	} 
	else {
		for (unsigned i = 0; i < vertice.count; i++) {
			float value = data.values[i];
			if (!data.normalized)
				value = normalizeValue(value);
			GLcolor color = valToColor(value, data.colormap);
			glColor3f(color.R, color.G, color.B);
			glDrawArrays(GL_POINTS, i, 1);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
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
	glColor3f(0.2f, 0.2f, 0.2f);
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

void ModelObject::render(bool forceVertices) 
{
	if (vertice.bid == 0)
		return;

	glPushMatrix();
	glScalef(factor.scale, factor.scale, factor.scale);
	glTranslatef(factor.translate[0], factor.translate[1], factor.translate[2]);
	
	if (shader.show) 
		renderShader();
	else 
	{
		if (edges.show && texture.show) 
			renderTextured();
		else {
			if (edges.show)
				renderEdges();

			if (data.show || data.useBuffer)
				renderColoredPoints();
			else
				renderPoints();	
		} 
	}
	if (forceVertices) {
		glEnable(GL_DEPTH_TEST);
		if (data.show || data.useBuffer)
			renderColoredPoints();
		else
			renderPoints();	
		glDisable(GL_DEPTH_TEST);
	}
	glPopMatrix();
};

void ModelObject::renderShader() {
	
	glUseProgram(this->shader.programId);
	
	for (unsigned i=0; i<attrib.size(); i++) {
		glEnableVertexAttribArray(attrib[i].location);
		glBindBuffer( GL_ARRAY_BUFFER, attrib[i].buffer);
		glVertexAttribPointer(attrib[i].location, attrib[i].size, attrib[i].type, GL_FALSE, 0, NULL);
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertice.bid);
	glVertexPointer(3, vertice.type, 0, NULL);


	if (edges.show) {
		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edges.bid);
		glDrawElements(edges.mode, edges.count, edges.type, 0);
		glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	}

	glPointSize(4.0f);
	glDrawArrays(GL_POINT, 0, vertice.count);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
	glPopMatrix();
}

unsigned ModelObject::getColormap() {
	return data.colormap;	
};

bool ModelObject::willDrawColored() {
	return (!edges.show && !shader.show);	
};


#pragma endregion Model Object

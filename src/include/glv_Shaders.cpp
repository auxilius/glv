#include "glv_Shaders.h"
#include <windows.h>


Shaders *shader = NULL;

GLchar * vertBoxRender[] = {
	"#version 130 \n"
	"uniform vec4 corners, color; \n"
	"uniform int width, height; \n"
	"out vec4 fragColor; \n"
	"void main(void) { \n"
	"	fragColor = color; \n"
	"	vec2 vertex = corners.xy; \n"
	"	if (gl_VertexID==1) vertex.x = corners.z; \n"
	"	if (gl_VertexID==2) vertex = corners.zw; \n"
	"	if (gl_VertexID==3) vertex.y = corners.w; \n"
	"	vertex = 2.0 * vertex / vec2(width, height) - 1.0; \n"
	"	gl_Position = vec4(vertex.x, -vertex.y, 0.0, 1.0); \n"
	"} \n"
};
GLchar * fragBoxRender[] =  { 
	"#version 130 \n"
	"in vec4 fragColor; \n"
	"void main(void) { \n"
	"	gl_FragColor = fragColor; \n"
	"} \n" 
};


GLchar * vertModelRender[] = {
	"#version 130 \n"
	"in vec3 inVertex; \n"
	"in vec2 inCoords; \n"
	"in vec3 inValueColor; \n"
	"uniform int enableValues, enableTexture; \n"
	"uniform mat4 glvModelMatrix, glvViewMatrix, glvProjectionMatrix; \n"
	"out vec2 coords; out vec3 valueColor; \n"
	"void main(void) { \n"
	"	if (enableTexture>0) coords = inCoords; \n"
	"	if (enableValues>0) valueColor = inValueColor; \n"
	"	gl_Position = glvProjectionMatrix * glvViewMatrix * glvModelMatrix * vec4(inVertex, 1.0); \n"
	"} \n"


};
GLchar * fragModelRender[] =  { 
	"#version 130 \n"
	"uniform int enableValues, enableTexture; \n"
	"uniform sampler2D texData; \n"
	"uniform vec4 color; \n"
	"in vec2 coords; in vec3 valueColor; \n"
	"void main(void) { \n"
	"	vec4 fragColor = color; \n"
	"	if (enableTexture>0) fragColor = texture(texData, coords); \n"
	"	if (enableValues>0) fragColor = vec4(valueColor, 1.0); \n"
	"	gl_FragColor = fragColor; \n"
	"}  \n"
};


Shaders::Shaders() {
	actualProgram = 0;
};

void Shaders::init() {
	program[progRenderBox] = loadProgram(vertBoxRender[0], fragBoxRender[0]);
	program[progRenderModel] = loadProgram(vertModelRender[0], fragModelRender[0]);
};

void Shaders::bind(ShaderProgram programName) {
	actualProgram = program[programName];
	glUseProgram(program[programName]);
};

void Shaders::unbind() {
	actualProgram = 0;
	glUseProgram(0);
};

bool Shaders::setUniform1i(std::string name, GLint value) {
	GLint location = getLocationUniform(name);
	if (location == -1)
		return false;
	glUniform1i(location, value);
	return true;
};

bool Shaders::setUniform1f(std::string name, GLfloat value) {
	GLint location = getLocationUniform(name);
	if (location == -1)
		return false;
	glUniform1f(location, value);
	return true;
};

bool Shaders::setUniform4f(std::string name, GLfloat* values) {
	GLint location = getLocationUniform(name);
	if (location == -1)
		return false;
	glUniform4fv(location, 1, values);
	return true;
};

bool Shaders::setUniformMatrix4f(std::string name, GLfloat* values) {
	GLint location = getLocationUniform(name);
	if (location == -1)
		return false;
	glUniformMatrix4fv(location, 1, GL_FALSE, values);
	return true;
};

bool Shaders::setAttribute(std::string name, GLuint buffer, GLint size, GLenum type) {

	GLint location = getLocationAttrib(name.c_str());
	if (location != -1) {
		glEnableVertexAttribArray(location);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(location, size, type, GL_FALSE, 0, NULL);
		return true;
	}
	return false;
};

bool Shaders::setAttribute(std::string name, GLfloat *arr, GLint size) {

	GLint location = getLocationAttrib(name.c_str());
	if (location != -1) {
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, arr);
		return true;
	}
	return false;
};

GLint Shaders::getLocationUniform(std::string name) {
	if (actualProgram <= 0)
		return -1;
	return glGetUniformLocation(actualProgram, name.c_str());
};

GLint Shaders::getLocationAttrib(std::string name) {
	if (actualProgram <= 0)
		return -1;
	return glGetAttribLocation(actualProgram, name.c_str());
};


// load and compile the shader source and return it's ID
GLuint Shaders::loadShader(GLenum shaderType, GLchar * shaderSource) {
	
	if (shaderSource == NULL)
		return 0;

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {

		GLchar log[1024];
		GLsizei length;
		glGetShaderInfoLog(shader, 1024, &length, log);
		MessageBox(	NULL, log, "GLV shader compilation error", MB_ICONERROR | MB_OK );
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

// link vertex and fragment shaders and create the program
GLuint Shaders::loadProgram(GLchar * vertexShaderSource, GLchar * fragmentShaderSource) {
	// nacitanie a kompilacia shaderov
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (vertexShader == 0)
		return 0;

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader);
		return 0;
	}

	GLuint programObj = glCreateProgram();
	glAttachShader(programObj, vertexShader);
	glAttachShader(programObj, fragmentShader);
	glLinkProgram(programObj);

	GLint status;
	glGetProgramiv(programObj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {

		GLchar log[1024];
		GLsizei length;
		glGetProgramInfoLog(programObj, 1024, &length, log);
		MessageBox(	NULL, log, "GLV shader linking error", MB_ICONERROR | MB_OK );
		glDeleteProgram(programObj);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		return 0;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	return programObj;
}
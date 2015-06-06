#pragma once
#ifndef GLV_H_SHADERS
#define GLV_H_SHADERS

#include <string>
#include <vector>
#include "gl/glew.h"

enum ShaderProgram { progRenderBox, progRenderModel, progRenderTexture };

class Shaders {
public:
	Shaders();
	void init();

	void bind(ShaderProgram programName);
	void bindCustom(GLuint custom_program);
	void unbind();
	
	bool setUniform1i(std::string name, GLint value);
	bool setUniform1f(std::string name, GLfloat value);
	bool setUniform4f(std::string name, GLfloat* values);
	bool setUniformMatrix4f(std::string name, GLfloat* values);

	bool setAttribute(std::string name, GLuint buffer, GLint size, GLenum type = GL_FLOAT);
	bool setAttribute(std::string name, GLfloat *arr, GLint size);
	bool setAttribute(GLuint location, GLuint buffer, GLint size, GLenum type = GL_FLOAT);
	void disableAttributes();
private:
	std::vector<GLuint> activeAttributes;
	GLuint program[3];
	GLuint actualProgram;
	GLint getLocationUniform(std::string name);
	GLint getLocationAttrib(std::string name);

	GLuint loadShader(GLenum shaderType, GLchar * shaderSource);
	GLuint loadProgram(GLchar * vertexShaderSource, GLchar * fragmentShaderSource);

	void enableAttribute(GLuint location);
	
};


extern Shaders *shader;


#endif

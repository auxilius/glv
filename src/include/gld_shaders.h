#ifndef H_GLD_SHADERS
#define H_GLD_SHADERS

#include <string>
#include <gl/glew.h>

bool shaderFileAddToProg(GLuint shaderProgram, std::string fileName, GLenum shaderType);
bool shaderAddToProg(GLuint shaderProgram, GLchar* shaderString, GLenum shaderType);

#endif
#ifndef H_GLD_SHADERS
#define H_GLD_SHADERS

#include <string>
#include <gl/glew.h>

bool shaderAddToProg(GLuint shaderProgram, std::string fileName, GLenum shaderType);

#endif
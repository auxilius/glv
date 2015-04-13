#include "gld_shaders.h"

#include <iostream>
#include <string>
#include <GL/glew.h>

#include "gld_constants.h"
#include "gld_types.h"


GLchar* readShaderFile(std::string fileName)
{
	char * fName = stringToChar(fileName);
	if (!fileExists(fName)) {
		std::cout << "GLD ERROR: Shader file does not exist " << fName << std::endl;
		MessageBox(0, ERRORTEXT_SHADERFILE_DONT_EXIST, ERRORTEXT_HEADER, MB_OK | MB_ICONWARNING);
	}
	
    FILE* file;
	fopen_s(&file, fName, "r"); 
    
	if( file == NULL ) {
        std::cout << "GLD ERROR: Cannot open file " << fName << std::endl;
		return NULL;
    }

    // ziska velkost suboru v bajtoch
    fseek(file, 0L, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // nacita data zo suboru
    GLchar* buffer = new GLchar[fileSize + 1];
	int bytes = fread(buffer, 1, fileSize, file);
    buffer[bytes] = 0;
	fclose(file);

	return buffer;
}

bool shaderCompileTo(GLuint shaderProgram, char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0) {
        std::cout << "GLD ERROR: Error creating shader." << std::endl;
        return false;
    }
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        std::cout << "GLD ERROR: Error compiling shader" << ": '" << InfoLog << "'" << std::endl;
		MessageBox(0, ERRORTEXT_SHADER_COMPILE_ERROR, ERRORTEXT_HEADER, MB_OK | MB_ICONWARNING);
        return false;
    }
    glAttachShader(shaderProgram, ShaderObj);
	return true;
}

bool shaderAddToProg(GLuint shaderProgram, std::string fileName, GLenum shaderType)
{
	GLchar* shaderString = readShaderFile(fileName);
	
	shaderCompileTo(shaderProgram, shaderString, shaderType);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << "GLD ERROR: Error linking shader program: " << ErrorLog << std::endl;
        return false;
	}

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << "GLD ERROR: Invalid shader program: " << ErrorLog << std::endl;
        return false;
    }

	return true;
}

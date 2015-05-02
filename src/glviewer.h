#ifndef __OPENGL_VIEWER
#define __OPENGL_VIEWER

#include <windows.h>
#include <gl/glew.h>
#include <string>

namespace glv {

	/* Initializes the library. Needs to be called before any buffer generation, after OpenGL context creation. */
	int init(std::string workingDir, HGLRC glrcToShare = NULL, HDC dcToShare = NULL);

	/* Shows a window */
	void show();

	/* Hides a viewing window */
	void hide();

	/* Adds a texture to the library, Caption and an texture ID must be specified. */
	bool setTexture(std::string caption, GLuint texture);

	/* Add a line to the text output list. specify a format string and an array of pointers. Output format control signs: bool(%b), int(%i), unsigned(%u), float(%f), double(%d), char(%c). */
	bool setLine(std::string format, void * data);
	bool setLine(std::string format, void * data[]);
	bool setLine(std::string caption, std::string format, void * data);
	bool setLine(std::string caption, std::string format, void * data[]);

	/* Add a vertex cloud rendering form for a model */
	bool setModel(std::string caption, unsigned count, GLuint vertices, GLenum type = GL_FLOAT);

	/* Specifyes a conetivity between vertices */
	bool modelEdges(std::string caption, GLenum mode, unsigned count, GLuint indices, GLenum type = GL_UNSIGNED_INT);

	/* Assign texture id and coordinates for vertices */
	bool modelTexture(std::string caption, GLuint texture, GLuint coordinates, GLenum type = GL_FLOAT);


	/* UNDER WORK - note that these may not be working correctly! */ 

	const int CLMAP_RAINBOW = 0;
	const int CLMAP_BLUERED = 1;

	/* Assign a per vertex data array to the vertex buffer */
	bool modelData(std::string caption, float * data, float min = 0.0f, float max = 1.0f, int colorMap = CLMAP_RAINBOW );

	/* Assign a per vertex data array to the vertex buffer and create a collor buffer to increase render efficiency */
	bool modelColor(std::string caption, float * data, float min = 0.0f, float max = 1.0f, int colorMap = CLMAP_RAINBOW );

	/* Assign a per vertex color buffer to the model. */
	bool modelColorBuffer(std::string caption, GLuint colorBuffer );

	/* Assign a vertex-normal buffer */
	bool modelNomals(std::string caption, GLuint normalBuffer );

	/* Assign a shader program which will be rendering a model in viewer window */
	bool modelShaderProgram(std::string caption, GLuint shaderProgramId);

	/* Assign vertex attribute list to the model */
	bool modelVertexAttrib(std::string caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer);
	
};


#endif
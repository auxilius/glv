#ifndef H_GLD_VISUALIZER_MODEL_OBJECT
#define H_GLD_VISUALIZER_MODEL_OBJECT

#include "gld_types.h"
#include "glv_Shaders.h"
#include <glm/glm.hpp>

namespace {

	struct MOFactor {
		GLfloat scale;
		GLfloat translate[3];
		bool isCalculated;
	};

	struct MOVertices {
		GLuint bid;
		unsigned count;
		GLenum type;
	};

	struct MONormals {
		GLuint bid;
		bool show;
	};

	struct MOEdges {
		GLuint bid;
		GLenum mode;
		GLenum type;
		unsigned count;
		bool show;
	};

	struct MOTexture {
		GLuint coords;
		GLuint id;
		GLenum type;
		bool show;
	};

	struct MOData {
		float* values;
		int colormap;
		float minValue, maxValue;
		bool normalized;
		bool show;

		GLuint color_bid;
		bool useBuffer;
	};

	struct MOShader {
		GLuint programId;
		bool show;
	};

	struct MOAttrib {
		GLuint buffer;
		GLuint location;
		GLint size;
		GLenum type;
	};

}


class ModelObject {
public:
	glm::mat4 getMatrix(void);
	void setAttributes(Shaders *shader);
	void render(Shaders *shader); 

private:
	GLfloat* valuesToColors(void);

	void drawElements(Shaders *shader);
	void drawVertices(Shaders *shader);


public:

	//ModelObject();
	ModelObject(std::string capt, unsigned vertCount, GLuint verts, GLenum dataType);
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vertice.bid; };
	void setVertices(unsigned count, GLuint verts, GLenum dataType);
	void setData(float* P, float min, float max, int cl_map);
	void setColor(float* P, float min, float max, int cl_map);
	void useColorBuffer(GLuint bid);
	void setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void setTexture(const GLuint tex, const GLuint coords, GLenum type);
	void setNormals(const GLuint bufferid);
	void setShader(const GLuint shaderProgramId);
	void addVertexAttrib(GLuint location, GLint size, GLenum type, GLuint buffer);
	unsigned getColormap();
	bool willDrawColored();
	void render(bool forceVertices);


public://private:
	std::string	caption;
	MOFactor factor;
	MOVertices vertice;
	MONormals normals;
	MOEdges edges;
	MOTexture texture;
	MOData  data;
	MOShader shader;
	std::vector<MOAttrib> attrib;

	GLuint defaultShaderProgram;
	//float normalizeValue(float value);
	void calculateTransformation();
	//void renderPoints();
	//void renderColoredPoints();
	//void renderNormals();
	//void renderTextured();
	//void renderEdges();
	//void renderShader();
};

#endif
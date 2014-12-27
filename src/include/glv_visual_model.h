#ifndef H_GLD_VISUALIZER_MODEL_OBJECT
#define H_GLD_VISUALIZER_MODEL_OBJECT

#include "gld_types.h"

class ModelObject {
private:

	std::string	caption;

	struct MOFactor {
		GLfloat scale;
		GLfloat translate[3];
		bool isCalculated;
	} factor;

	struct MOVertices {
		GLuint bid;
		unsigned count;
		GLenum type;
	} vertice;

	struct MOEdges {
		GLuint bid;
		GLenum mode;
		GLenum type;
		unsigned count;
		bool show;
	} edges;

	struct MOTexture {
		GLuint coords;
		GLuint id;
		GLenum type;
		bool show;
	} texture;

	struct MOData {
		float* values;
		int colormap;
		float minValue, maxValue;
		bool normalized;
		bool show;

		GLuint color_bid;
		bool useBuffer;
	} data;

	struct MOShader {
		GLuint programId;
		bool show;
	} shader;

	struct MOAttrib {
		GLuint buffer;
		GLuint location;
		GLint size;
		GLenum type;
	};
	std::vector<MOAttrib> attrib;

	GLuint defaultShaderProgram;
	bool createDefaultShader();
	float normalizeValue(float value);
	void calculateTransformation();
	void renderPoints();
	void renderColoredPoints();
	void renderTextured();
	void renderEdges();
	void renderShader();

public:

	ModelObject();
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vertice.bid; };
	void set(std::string C, unsigned N, unsigned VID, GLenum type);
	void setData(float* P, float min, float max, int cl_map);
	void setColor(float* P, float min, float max, int cl_map);
	void useColorBuffer(GLuint bid);
	void setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void setTexture(const GLuint tex, const GLuint coords, GLenum type);
	void setShader(const GLuint shaderProgramId);
	void addVertexAttrib(GLuint location, GLint size, GLenum type, GLuint buffer);
	void render();
};

#endif
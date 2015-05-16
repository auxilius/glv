#ifndef H_GLD_VISUALIZER_MODEL_OBJECT
#define H_GLD_VISUALIZER_MODEL_OBJECT

#include "gld_types.h"
#include "glv_Shaders.h"
#include <glm/glm.hpp>

namespace {

	struct MOVertices {
		GLuint bid;
		unsigned count;
		GLenum type;
	};

	struct MONormals {
		GLuint bid;
		bool exist;
	};

	struct MOEdges {
		GLuint bid;
		GLenum mode;
		GLenum type;
		unsigned count;
		bool exist;
	};

	struct MOTexture {
		GLuint coords;
		GLuint id;
		GLenum type;
		bool exist;
	};

	struct MOData {
		float* values;
		int colormap;
		float minValue, maxValue;
		bool normalized;
		bool exist;

		GLuint color_bid;
		bool useBuffer;
	};

	struct MOShader {
		GLuint programId;
		bool exist;
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
	// constructor
	ModelObject(std::string capt, unsigned vertCount, GLuint verts, GLenum dataType);
	// render the model with used mode
	enum RenderMode {rmVertices=0, rmEdges=1, rmSolid=2, rmTextured=3};
	RenderMode getMaximalRenderMode();
	void onRender(Shaders *shader, RenderMode mode);
	void onRenderNormals(Shaders *shader);

public: // Property SETters
	// Vertices
	void setVertices(unsigned count, GLuint verts, GLenum dataType, bool rewrite);
	// Elements
	void setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	// Texture
	void setTexture(const GLuint tex, const GLuint coords, GLenum type);
	// Normals
	void setNormals(const GLuint bufferid);
	// Pervertex Data
	void setData(float* P, float min, float max, int cl_map);
	// Color 'n Color Buffer
	void setColor(float* P, float min, float max, int cl_map);
	void useColorBuffer(GLuint bid);
	// Custom Shader
	void setShader(const GLuint shaderProgramId);
	// Custom Attributes
	void setVertexAttrib(GLuint location, GLint size, GLenum type, GLuint buffer);

public: // Property GETters
	// caption of the model
	std::string	getCaption() { return caption; };
	// returns id of the shader
	GLuint	getCustomProgram();
	// normalization transformation matrix
	glm::mat4 getMatrix(void) { return matrix; };
	// colormap for data visualization
	unsigned getColormap();

private: // recorded properties
	std::string	caption;
	glm::mat4 matrix;
	MOVertices vertice;
	MONormals normals;
	MOEdges edges;
	MOTexture texture;
	MOData  data;
	MOShader shader;
	std::vector<MOAttrib> attrib;
	
	GLfloat* valuesToColors(void);
	void calculateMatrix();
	void drawElements();

};

#endif
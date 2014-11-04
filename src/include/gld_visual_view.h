#ifndef H_GLD_VISUALIZER_VIEW
#define H_GLD_VISUALIZER_VIEW

#include "gld_types.h"

class View {
public:
	View(){};
	Box border;
	unsigned layer;
	void drawBackground(float r, float g, float b);
	void drawBorder(float r, float g, float b);
	void setBorder(Box newBorder);
};


class TextureObject {
private:
	std::string caption;
	GLuint bufferID;
	GLint width, height;
	float ratio;
public:
	TextureObject() : caption(""), bufferID(0) {};
	TextureObject(std::string C, GLuint ID);
	void set(GLuint ID);
	std::string getCaption() { return caption; };
	void render(Box frame);
};

class TextureView : public View {
private:
	std::vector<TextureObject> * list;
	int index;
	TextureObject * getTexture();
public:
	std::string waitingForTextureCaption;
	TextureView() : index(-1), list(NULL) {};
	TextureView(Box cBorder) : index(-1), list(NULL) { border = cBorder; };
	void setTextureList(std::vector<TextureObject> * pointer);
	void showTexture(int id);
	std::string getTextureCaption();
	void render();
};

void valToColor_Rainbow(float value);
void valToColor_BlueRed(float value);

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
		float minValue, maxValue;
		bool normalized;
		bool show;
	} data;
	struct MOShader {
		GLuint programId;
		bool show;
	} shader;
	float normalizeValue(float value);
	void calculateTransformation();
	void renderPoints();
	void renderColoredPoints();
	void renderTextured();
	void renderEdges();
public:
	ModelObject();
	std::string	getCaption() { return caption; };
	GLuint getVBO() { return vertice.bid; };
	void set(std::string C, unsigned N, unsigned VID, GLenum type);
	void setData(float* P, float min, float max);
	void setIndices(const GLenum mode, const unsigned count, const GLuint indices, GLenum type);
	void setTexture(const GLuint tex, const GLuint coords, GLenum type);
	void setShader(const GLuint shaderProgramId);
	void setShader(std::string shaderFile, GLenum shaderType);
	void render();
};

class ModelView : public View {
private:
	std::vector<ModelObject> * modelList;
	int modelListIndex;
	bool wasSelected;
	Point lastMousePos;
	float normalizeValue(float value);
	void resetCamera();
public:
	double hang, vang, dist;
	std::string waitingForModelCaption;
	ModelView();
	ModelView(Box cBorder);
	void showModel(int index);
	std::string getModelCaption();
	ModelObject * getModel();
	void draw();
	void onMouseMove(int x, int y);
	void onMouseDown(mouseButton button);
	void onMouseUp(mouseButton button);	
	void onMouseWheel(signed short direction);
	void setModelList(std::vector<ModelObject> * list) {
		modelList = list;
	};
};



#endif
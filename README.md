# OpenGL View Library (GLV)
GLV is a library for the visualization of OpenGL textures, models or other resources. The library is adding a possibility for users to open additional window beside their program, which can be then used for displaying a model in multiple ways, textures or variable values.

The User can configure window layout, to decide what to displayinthewindow. The library also supports creating more layouts and switching between them at runtime. User is free to apply his own shaders and vertex attributes for in-
dividual objects, to customize visualization. Library can be used as a supportive visualizer or tool for debugging
OpenGL applications.

## Dependencies
Library is using OpenGL Mathematics which can be found at http://glm.g-truc.net/0.9.6/index.html

## Code Example
to use a library, interface is provided in file glviewer.h

```
void main(void) {

  HGLRC myGLRC;
  HDC myDC;

  myStartOpenGL(&myGLRC, &myDC); // your procedure for context cration
  
  // initialize the viewing window and set path where files will be stored
  glv::init("./glv_files/", myGLRC, myHDC); 
  glv::show(); // show the viewer window
  
  // create some buffer with 8 vertices => cube
  GLuint vertBuf = myCubeVertexBuffer(); 
  // add a model and assign it with caption
  glv::setModel("Test Cube Model", 8, vertBuff); 
  
  // get the buffer with cube indexes => 16 triangles for cube
  GLuint indexBuf = myCubeIndexBuffer(); 
  // specify how elements will be rendered in the viewer
  glv::modelElement("Test Cube Model", GL_TRIANGLES, 16, indexBuff); 
  
  // load your texture
  Gluint texID = myLoadNiceTexture(); 
  // enable to display a texture defining it's ID
  glv::setTexture("Some Nice Texture", texID); 
  
  // set a value tracker to watch the variable 'texID'
  glv::setLine("Texture ID", "texture id is %u", &texID);
  
  while(true)
    ; // loop this thread forever and view resources
    
};
```

## Interface
`glv::init(glvWorkingDir, glrcToShare, dcToShare);`

Used for initialization of the library. glvWorkingDir is directory where all supplementary files will be saved. Before this call, GLEW must be initialized. 

`glv::show();` and `glv::hide();`

The functions for window open/close manipulation.

#### Textures
`glv::setTexture("My Test Texture", textureID);`

Adds new texture to the list. If there already is one with the same caption, overwrites it wit this new textureID.

#### Variable Watches
`glv::setLine("%f.0 FPS", &fps)`

Set a new wariable watch line. First parameter defines the form of the line. The control sign '%d', '%f', '%b' specify the output format (decimal, float, boolean). Second parameter is the value array, where all individual pointers are stored. The size of the array must be the same as the number of '%' signes in the control string.

#### Models
`std::string myModel = "My Test Triangle Model";`

Firstly, create a name for a model. This name will be then used for identification of the model when other properties will be added.

`glv::setModel(myModel, 3, verts);`

Create model in the viewer. Pass number of vertices and a buffer ID where the positions are stored.

`glv::modelElement(myModel, GL_TRIANGLES, 1, indices);`

Specify conectivity of the elements. Pass rendering mode, number of elements and ID of buffer where indices are stored.

`glv::modelTexture(myModel, textureID, coords);`

Assign texture to the model. Pass ID of a texture and buffer ID where coordinates are stored.

`bool modelData(std::string caption, float * data, float min, float max, int colorMap);`

When there is a need to display per vertex values mapped to the color-scale, pass pointer to the data, minimal value (default 0.0), maximal value (default 1.0) and color scale parameter. For now there are available these color-scales:
```
glv::CLMAP_RAINBOW; // all colors of a rainbow mapped to the scale
glv::CLMAP_BLUERED; // lowest numbers mapped to blue, highest to the red
```

#### Using Custom Shaders for Models

`glv::modelAttribs(myModel, location, numPerVert, buffer);`

By this function the unlimited amount of attribute buffers can be sent. These attributes can be identified by their 'location' later in the shader.

`glv::modelProgram(myModel, programID);`

Specify the shader program for the model. All rendering of the model will be done with usage of this shader.

For synchronization with library, multiple constants can be used such as:
`uniform mat4 glvModelMatrix, glvViewMatrix, glvProjectionMatrix;`
using these uniforms in custom shader program, MVP matrix can be build.

###### Example of Shader Program

```
#version 430

uniform mat4 glvModelMatrix, glvViewMatrix, glvProjectionMatrix;
layout (location = 0) in vec3 inVertex;

void main(void) {
  gl_Position = MVP * inVertex;
};
```

this shader was used beside following glv interface setup:
```
std::string caption = "Model With Shader";
glv::setModel(caption, model->vertexCount, 0);
glv::modelEdges(caption, GL_TRIANGLES, model->elementCount, model->indices);
glv::modelVertexAttrib(caption, 0, 3, GL_FLOAT, model->vertexPositions);
glv::modelShaderProgram(caption, shaderProg);
```
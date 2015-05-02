# OpenGL View Library (GLV)
GLV is a library for the visualization of OpenGL textures, models or other resources. The library is adding a possibility for users to open additional window beside their program, which can be then used for displaying a model in multiple ways, textures or variable values.

The User can configure window layout, to decide what to displayinthewindow. The library also supports creating more layouts and switching between them at runtime. User is free to apply his own shaders and vertex attributes for in-
dividual objects, to customize visualization. Library can be used as a supportive visualizer or tool for debugging
OpenGL applications.

## Code Example
to use a library, interface is provided in file gldebugger.h

```
void main(void) {

  HGLRC myGLRC;
  HDC myDC;

  myStartOpenGL(&myGLRC, &myDC); // your procedure for context cration
  
  glv::init("./glv_files/", myGLRC, myHDC); // initialize the viewing window and set path where files will be stored
  glv::show(); // show the viewer window
  
  GLuint vertBuf = myCubeVertexBuffer(); // create some buffer with 8 cube vertices
  glv::setModel("Test Cube Model", 8, vertBuff); // add a model and assign it with caption
  
  GLuint indexBuf = myCubeIndexBuffer(); // get the buffer with cube indexes - 16 triangles
  glv::modelElement("Test Cube Model", GL_TRIANGLES, 16, indexBuff); // specify how elements will be rendered in the viewer
  
  Gluint texID = myLoadNiceTexture(); // load some your texture
  glv::setTexture("Some Nice Texture", texID); // enable to display a texture defining it's ID
  
  glv::setLine("Texture ID", "texture id is %u", &texID); // set a value tracker to watch the variable 'texID'
  
  while(true)
    ; // loop this thread forever and view resources
    
};
```

## Interface
`glv::init(glvWorkingDir, glrcToShare, dcToShare);`

Used for initialization of the library. glvWorkingDir is directory where all supplementary files will be saved.

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

`glv::setModel(myModel, 3, *verts);`

###### Default Rendering
`glv::modelElement(myModel, GL_TRIANGLES, 1, indices);`

`glv::modelTexture(myModel, textureID, coords);`

`glv::modelNormals(myModel, normals);`

###### Shader Rendering
`glv::modelAttribs(myModel, location, numPerVert, buffer);`

By this function the unlimited amount of attribute buffers can be sent. These attributes can be identified by their 'location' later in the shader.

`glv::modelProgram(myModel, programID);`

Specify the shader program for the model. All rendering of the model will be done with usage of this shader.


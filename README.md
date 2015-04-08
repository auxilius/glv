# OpenGL View Library (GLV)
GLV is a library for the visualization of OpenGL textures, models or other resources. The library is adding a possibility for users to open additional window beside their program, which can be then used for displaying a model in multiple ways, textures or variable values.

The User can configure window layout, to decide what to displayinthewindow. The library also supports creating more layouts and switching between them at runtime. User is free to apply his own shaders and vertex attributes for in-
dividual objects, to customize visualization. Library can be used as a supportive visualizer or tool for debugging
OpenGL applications.

## Interface
`glv::init();`

Used for initialization of the library.

`glv::openWindow();` and `glv::closeWindow();`

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


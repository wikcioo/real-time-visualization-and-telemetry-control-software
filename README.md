## Real-Time Visualization and Telemetry Control Software

### TODOs:

Main:
- [x] creates and starts application

Application:
- [x] holds a window
- [x] initializes a window
- [x] starts an application loop which calls a renderer in the window

Window:
- [x] controls if the application is running
- [x] initializes window parameters
- [ ] contains opengl context and opengl ui context
- [ ] initializes contexts
- [x] sets input callbacks

Renderer:
- [x] OpenGL Buffer Manager
    - [x] stores VAO, VBO, EBO
    - [x] creates VAO from vertices and indices
    - [x] frees (destroy) stored VAO, VBO, EBO
    - [x] binds/unbinds VAO
    - [x] draws from memory via VAO
- [x] OpenGL Framebuffer Manager
    - [x] stores FBO and TextureID
    - [x] creates FBO with given width and height
    - [x] frees resources stored in FBO and TextureID
    - [x] binds/unbinds framebuffer
- [x] OpenGL Context
    - [x] gets window pointer
    - [x] initializes glew
    - [x] prepares for rendering
- [ ] OpenGL UI Context
    - [ ] initializes imgui context
    - [ ] prepares for rendering
    - [ ] renders what's stored in imgui frame (populated somewhere else)
    - [ ] frees resources

Camera:
- [ ] keeps view matrix and projection
- [ ] has position, direction and other vectors
- [ ] fov and acpect ratios
- [ ] updates vectors when processing input

Shader:
- [x] support vertex and fragment shaders
- [x] creates shader program from shader text files
- [x] binds/unbinds the program
- [x] hot-reloads the program with new shader source code
- [x] sets uniforms
- [ ] optimize uniforms by using cache

Mesh:
- [ ] stores the buffer manager
- [ ] stores vertices and indices which the buffer manager uses
- [ ] loads meshes from files (using assimp)
- [ ] draws using buffer manager

Entity:
- [ ] contains of mesh, shader and its translations

Scene:
- [ ] contains camera, framebuffer, entities, lighting information
- [ ] during rendering prepares framebuffer and new imgui scene filled with texture from the ui context

Control Panels:
- [ ] render imgui widgets
- [ ] gets an entitie from the scene to modify it's translations

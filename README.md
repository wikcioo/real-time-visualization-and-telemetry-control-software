# Real-Time Visualization and Telemetry Control Software
![Application screenshot](docs/public/screenshots/screenshot_v01.png?raw=true)

## How to build?
**Note: this project has only been tested on Linux**

### Install depencencies
On Arch based distributions:
```sh
sudo pacman -S base-devel glfw-x11 glm
```

### Build
```sh
make
```

### Run
```sh
./build/ccs
```
<br>

### Features:
Main:
- [x] creates and starts application

Application:
- [x] holds a window
- [x] initializes a window
- [x] starts an application loop which calls a renderer in the window

Window:
- [x] controls if the application is running
- [x] initializes window parameters
- [x] contains opengl context and opengl ui context
- [x] initializes contexts
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
- [x] OpenGL UI Context
    - [x] initializes imgui context
    - [x] prepares for rendering
    - [x] renders what's stored in imgui frame (populated somewhere else)
    - [x] frees resources

Camera:
- [x] keeps view matrix and projection
- [x] has position, direction and other vectors
- [x] fov and acpect ratios
- [x] updates vectors when processing input

Shader:
- [x] support vertex and fragment shaders
- [x] creates shader program from shader text files
- [x] binds/unbinds the program
- [x] hot-reloads the program with new shader source code
- [x] sets uniforms
- [ ] optimize uniforms by using cache

Mesh:
- [x] stores the buffer manager
- [x] stores vertices and indices which the buffer manager uses
- [ ] loads meshes from files (using assimp)
- [x] draws using buffer manager

Material:
- [x] keeps the material's shader
- [x] holds all relevant uniforms

Entity:
- [x] contains of mesh, main shader, material and its translations
- [x] methods to change translations
- [x] draws using shader and mesh

Scene:
- [ ] contains lighting information
- [x] contains camera, framebuffer, entities
- [x] during rendering prepares framebuffer and new imgui scene filled with texture from the ui context

Control Panels:
- [x] render imgui widgets
- [x] gets an entitie from the scene to modify it's translations

Serial:
- [x] establish connection with the serial port
- [x] enable port configuration
- [x] display raw serial data
- [x] provide an interface to the port via a console
- [x] add instructions to connect to the device

Cube-related:
- [x] get G-force from 3 axis
- [x] display graphs of the G-force in 3 axis
- [x] parse the G-forces into pitch and roll angles that update the cube's model matrix

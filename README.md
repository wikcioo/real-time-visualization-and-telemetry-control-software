# Real-Time Visualization and Telemetry Control Software
![Application screenshot](docs/public/screenshots/screenshot_v02.png?raw=true)

Made with C++, OpenGL and ImGui

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

## Features:
- [x] Connect to serial device
- [x] Read data from serial port
- [x] Plot orientation in 3 axis
- [x] Visualization based on data
- [x] Arcball and fpv camera inside the scene

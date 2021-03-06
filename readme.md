# GFX

A work in progress OpenGL playground for learning different computer graphics concepts and tecniques.

### Prerequisites

This project has been tested on MacOS and makes use of multiple cross-platform compatible libraries like SDL2, GLM, ImGui or FreeType. [CMake](https://cmake.org/) is used as build system, while dependencies are managed by [Conan](https://conan.io/) package manager.

### Build

Create a _build_ folder:

```mkdir build && cd build```

From within the new created _build_ folder execute the following command to install the dependencies:

```conan install ..```

Build the project with CMake and from within the project root start the application:

```./build/bin/Gfx [--assets /assets/folder/path]```

### Asset sources and references

- [Learn OpenGL](https://learnopengl.com)
- [OpenGL Tutorial](http://www.opengl-tutorial.org)
- [Anton's OpenGL4 EBook](https://antongerdelan.net/opengl/#ebook)
- [C++ OpenGL Tutorial](https://www.youtube.com/playlist?list=PLysLvOneEETPlOI_PI4mJnocqIpr2cSHS)
- [Turbo Squid](https://www.turbosquid.com/)
- [3D Textures](https://3dtextures.me/)

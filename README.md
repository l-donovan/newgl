# newGL
newGL is an OpenGL playground

## Requirements
You will need:
* [`vcpkg`](https://vcpkg.io), for handling library requirements
* A modern C++ compiler and linker (GCC, Clang, MSVC, etc.)

To install the requisite packages:
```bash
$ vcpkg integrate install # If you haven't done so already
$ vcpkg install glfw3 glm freetype sdl2-image plog
```

You may need to add an architecture-specific triplet (i.e. `:x64-windows`) to the end of each package.


![Wireframe mode](misc/example1.png)

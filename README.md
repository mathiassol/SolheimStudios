# Solheim Studios
![License: All Rights Reserved](https://img.shields.io/badge/License-All%20Rights%20Reserved-red.svg)

![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake)


So after working on a Minecraft clone for a while, I decided to make more of a game/rendering engine. here is the core idea. I want a game engine that's almost like writing pure OpenGL, unlike say Unreal engine, you can code right in the project files, and it behaves almost like a boilerplate setup with a ui wrapper.

---
### building and compiling

_**windows:**_

First, install Visual Studio, vcpkg, and cmake. Yes VS not VS Code, during install make sure to select the desktop development with C++ workload. Also download cmake and make sure to add it to PATH.

https://visualstudio.microsoft.com/

https://cmake.org/

````
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

C:\vcpkg\vcpkg install glfw3 glew glm --triplet x64-windows
````

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
cmake --build . --config Release
```

## side note:
I will not continue work on this project, working on it for the time has helped me alot in understanding the structure of game engien, which indevently made me realize some important mistakes, so i will begin development on a new game engine. from a techincal stand point its due to the lack of ECS functionality.  
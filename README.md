# islay – A versatile framework for computer vision research
### About
**islay** is a easy-to-use visually-aesthetic framework for computer vision and image processing researches based on Dear ImGui.

Use imgui https://github.com/ocornut/imgui with cmake, opencv, opengl 3, sdl2

**islay** is designed to be cross-platform.

### How to Install
#### 1. Install dependencies
##### For Ubuntu users:
- OpenCV for image processing  
Install OpenCV as you like.
- Dependencies for Dear ImGui  
    - SDL2 for OpenGL library
    - GLFW for OpenGL library
    - GLM for math library for graphics
```Shell
$ sudo apt update # Be sure to update repository
$ sudo apt install -y libsdl2-dev
$ sudo apt install -y libx11-dev xorg-dev libglu1-mesa libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev
$ sudo apt install -y libglfw3 libglfw3-dev
$ sudo apt install -y libglew-dev
$ sudo apt install -y libglm-dev
```
##### For Mac users
*Homebrew* is a awesome package manager for Mac. Visit `https://brew.sh/index_ja` to install Homebrew.
- OpenCV for image processing  
Install OpenCV as you like.
- Dependencies for Dear ImGui  
    - SDL2 for OpenGL library
    - GLFW for OpenGL library
    - GLM for math library for graphics
```Shell
$ brew install sdl2
$ brew install glfw
$ brew install glm
```

#### 2. Configure CMakeLists.txt
Use `cmake-gui` is a easy way to go.  Set `OpenCV_DIR` to your OpenCV's installation (or build) directory and configure & generate.

#### 3. Build
```Shell
$ git clone https://github.com/mhirano/islay.git --recursive    
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Now you are ready to drink it neat;)
   
### Contact
 masahiro.dll_at_gmail.com (replace `_at_` with `@`)

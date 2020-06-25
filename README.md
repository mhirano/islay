# islay – A versatile framework for computer vision research
### About
based on Dear ImGui 

Use imgui https://github.com/ocornut/imgui with cmake, opencv, opengl 3, sdl2

- Islay is designed to be cross-platform.

### How to Install
#### 1. Install dependencies
##### For Ubuntu users:
- OpenCV for image processing
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
Replace following pathes for your environment.
|Name|Value|
----|----
|`OPENCV_EXTRA_MODULES_PATH`|`/path_to_your_contrib_module/`|
|`IPPROOT`|`/path_to_your_IPP_root_directory`|
|`WITH_IPP`|:white_square_button: (checked)|
|`BUILD_WITH_DYNAMIC_IPP`|:white_square_button: (checked)|
|`MKL_ROOT_DIR`|`/path_to_your_MKL_root_directory/`|
|`MKL_INCLUDE_DIRS`|`/path_to_your_MKL_include_directory/`|
|`MKL_WITH_TBB`|:white_square_button: (checked)|
|`WITH_TBB`|:white_square_button: (checked)|
|`TBB_ENV_INCLUDE`|`/path_to_your_TBB_include_directory`|
|`TBB_ENV_LIB`|`/path_to_your_libtbb.so/`|
|`TBB_ENV_LIB_DEBUG`|`/path_to_your_libtbb_debug.so/`|
|`WITH_VTK`|:white_square_button: (checked)|
|`VTK_DIR`|`/path_to_your_vtk_directory`|


#### 3. Build
```Shell
$ git clone https://github.com/mhirano/islay.git --recursive    
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Now you are ready to drink;)
   
### Contact
 masahiro.dll_at_gmail.com (replace `_at_` with `@`)

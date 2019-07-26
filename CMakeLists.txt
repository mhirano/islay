cmake_minimum_required(VERSION 3.5)
project(imgui_barebone)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW REQUIRED)

include_directories(
  include
  3rdparty/imgui
  3rdparty/imgui/examples
  3rdparty/imgui/examples/libs/gl3w
  ${OpenCV_INCLUDE_DIRECTORIES}
  ${SDL2_INCLUDE_DIRS}
)

add_library(imgui SHARED
  3rdparty/imgui/imgui.cpp
  3rdparty/imgui/imgui_demo.cpp
  3rdparty/imgui/imgui_draw.cpp
  3rdparty/imgui/imgui_widgets.cpp
  3rdparty/imgui/examples/imgui_impl_sdl.cpp
  3rdparty/imgui/examples/imgui_impl_opengl3.cpp
  3rdparty/imgui/examples/libs/gl3w/GL/gl3w.c
)
set_source_files_properties(imgui/examples/libs/gl3w/GL/gl3w.c PROPERTIES COMPILE_FLAGS -Wno-pedantic)

target_link_libraries(imgui
  ${OpenCV_LIBRARIES}
  ${PCL_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${GLFW_LIBRARIES}
  dl
)

add_executable(${PROJECT_NAME} src/main.cpp src/Application.cpp)
target_link_libraries(${PROJECT_NAME} imgui)

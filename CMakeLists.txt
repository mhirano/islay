cmake_minimum_required(VERSION 3.10)
project(islay)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR}/include)

# OpenCV
find_package(OpenCV REQUIRED HINTS ${OpenCV_DIR})
include_directories(${PROJECT_NAME} ${OpenCV_INCLUDE_DIRS} )
set(OpenCV_DIR "" CACHE PATH "OpenCV install path")

# Rendering
find_package(SDL2 REQUIRED)
find_package(GLFW REQUIRED)
find_package(glm REQUIRED)

# ImGui
include_directories(
  3rdparty/imgui
  3rdparty/imgui/examples
  3rdparty/imgui/examples/libs/gl3w
  ${SDL2_INCLUDE_DIRS}
)

######## ######## ######## ######## ######## ######## ######## ########
# Compiler settings
######## ######## ######## ######## ######## ######## ######## ########
# Check C++14 support and activate
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  set(CMAKE_CXX_STANDARD 14) # C++14...
  set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
  set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++14 support.")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g ")
message(STATUS "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O3 -DNDEBUG -march=native")
message(STATUS "${CMAKE_CXX_FLAGS_RELEASE}")
######## ######## ######## ######## ######## ######## ######## ########


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

if(UNIX AND NOT APPLE)
  target_link_libraries(imgui ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} GL dl )
else(APPLE)
  target_link_libraries(imgui ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} dl )
endif()

add_executable(${PROJECT_NAME} src/main.cpp src/Application.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} imgui)

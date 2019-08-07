cmake_minimum_required(VERSION 3.5)
project(imgui_barebone)


#### Environment
if (UNIX AND NOT APPLE) #Ubuntu...
  message(STATUS ">>> Linux")

  set(OpenCV_DIR /home/mhirano/opencv-4.1.1/build)
  set(Eigen_DIR /usr/include/eigen3)
  set(Qt5_DIR /opt/Qt/5.10.1/gcc_64)
  set(CMAKE_PREFIX_PATH ${Qt5_DIR})

  include_directories(/usr/include)
  link_directories(/usr/lib/x86_64-linux-gnu)

else(APPLE)
  message(STATUS ">>> Mac")

  # If you installed Qt with homebrew and 'brew link --force qt5', you don't have to set QTDIR for find_package
  # set(QTDIR /usr/local/Cellar/qt/5.10.0_1)
  # set(CMAKE_PREFIX_PATH ${QTDIR})

  set(OpenCV_DIR /Users/mhirano/opencv-3.4.1-tuned/release)
  set(Eigen_DIR /usr/local/include/eigen3)

  # Add include directory	(environemt specific)
  include_directories(/usr/local/include)

  # Add library directory
  # link_directories(/usr/local/Cellar/fftw/3.3.6-pl2/lib) # fftw3 via homebrew

endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR}/include)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS}  )

# Qt
find_package(Qt5 COMPONENTS Core REQUIRED)

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

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -pg")
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
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Qt5::Core imgui)

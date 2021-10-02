cmake_minimum_required(VERSION 3.18)

project(islay)

set(USE_EXTERNAL_DRIVE OFF CACHE BOOL "Use external drive for data")

if(USE_EXTERNAL_DRIVE)
  message(STATUS "LOAD DATA FROM EXTERNAL DRIVE")
  if(UNIX AND NOT APPLE)
    # Change
    set(RESOURCE_DIRECTORY /path_to_resource_directory CACHE PATH "Resource directory on external drive" FORCE)
    set(RESULT_PARENT_DIRECTORY /path_to_result_parent_directory CACHE PATH "Parent of result directory on external drive" FORCE)
  elseif(APPLE)
    set(RESOURCE_DIRECTORY /path_to_resource_directory CACHE PATH "Resource directory on external drive" FORCE)
    set(RESULT_PARENT_DIRECTORY /path_to_result_parent_directory CACHE PATH "Parent of result directory on external drive" FORCE)
  endif()
else()
  message(STATUS "LOAD DATA FROM INTERNAL DRIVE")
  set(RESOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/res CACHE PATH "Resource directory on internal drive" FORCE)
  set(RESULT_PARENT_DIRECTORY ${PROJECT_SOURCE_DIR}/result CACHE PATH "Parent of result directory on internal drive" FORCE)
endif()
#unset(USE_EXTERNAL_DRIVE CACHE)

#### Configure config files
message(STATUS "Configure json.in files")
file(GLOB JSON_IN_LISTS ${PROJECT_SOURCE_DIR}/config/*.json.in)
foreach(json_in_file IN LISTS JSON_IN_LISTS)
  message(STATUS "  Configuring ${json_in_file}")
  string(REPLACE .json.in .json json_file ${json_in_file})
  get_filename_component(json_file_name ${json_file} NAME_WE)
  configure_file(${json_in_file} "${json_file_name}.json")
  message(STATUS "  Configuring to ${PROJECT_BINARY_DIR}/${json_file_name}.json")
endforeach()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR}/include)

# put dependent libraries in ISLAY_LIBS
set(ISLAY_LIBS "")

# Threads
find_package(Threads REQUIRED)
set(ISLAY_LIBS "${ISLAY_LIBS};Threads::Threads")

# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED system filesystem)
if(Boost_FOUND)
  message(STATUS "!!! BOOST FOUND !!!")
  message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
  include_directories(${Boost_INCLUDE_DIRS})
  set(ISLAY_LIBS "${ISLAY_LIBS};${Boost_LIBRARIES}")
else()
  message(STATUS "!!! BOOST NOT FOUND !!!")
endif()

# OpenCV
find_package(OpenCV REQUIRED HINTS ${OpenCV_DIR})
include_directories(${PROJECT_NAME} ${OpenCV_INCLUDE_DIRS} )
set(OpenCV_DIR "" CACHE PATH "OpenCV install path")
set(ISLAY_LIBS "${ISLAY_LIBS};${OpenCV_LIBRARIES}")

# Eigen
find_package(Eigen3 REQUIRED)
set(ISLAY_LIBS "${ISLAY_LIBS};Eigen3::Eigen")

# rapidjson
include_directories(3rdparty/rapidjson/include)

# spdlog
include_directories(3rdparty/spdlog/include)

# Rendering
find_package(SDL2 REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# ImGui
include_directories(
  3rdparty/imgui
  3rdparty/imgui/examples
  3rdparty/imgui/examples/libs/gl3w
  ${SDL2_INCLUDE_DIRS}
)

# implot for Dear ImGui
include_directories( 3rdparty/implot )
set(IMPLOT_SRC
        3rdparty/implot/implot.h
        3rdparty/implot/implot_internal.h
        3rdparty/implot/implot.cpp
        3rdparty/implot/implot_items.cpp)

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
  ${IMPLOT_SRC}
)
set_source_files_properties(imgui/examples/libs/gl3w/GL/gl3w.c PROPERTIES COMPILE_FLAGS -Wno-pedantic)

if(UNIX AND NOT APPLE)
  target_link_libraries(imgui ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} GLEW::GLEW GL dl )
else(APPLE)
  target_link_libraries(imgui ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} GLEW::GLEW dl)
endif()
set(ISLAY_LIBS "${ISLAY_LIBS};imgui")

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Application.cpp
        src/Engine.cpp
        )
target_link_libraries(${PROJECT_NAME} ${ISLAY_LIBS})
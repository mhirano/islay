cmake_minimum_required(VERSION 3.22)

project(islay)

######## ######## ######## ######## ######## ######## ######## ########
# Project configuration
######## ######## ######## ######## ######## ######## ######## ########
## External drive configuration
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

## Configure config files
message(STATUS "Configure json.in files")
file(GLOB JSON_IN_LISTS ${PROJECT_SOURCE_DIR}/config/*.json.in)
foreach(json_in_file IN LISTS JSON_IN_LISTS)
  message(STATUS "  Configuring ${json_in_file}")
  string(REPLACE .json.in .json json_file ${json_in_file})
  get_filename_component(json_file_name ${json_file} NAME_WE)
  configure_file(${json_in_file} "${json_file_name}.json")
  message(STATUS "  Configuring to ${PROJECT_BINARY_DIR}/${json_file_name}.json")
endforeach()
######## ######## ######## ######## ######## ######## ######## ########


######## ######## ######## ######## ######## ######## ######## ########
# Compiler configuration
######## ######## ######## ######## ######## ######## ######## ########
# Check C++17 support and activate
include(CheckCXXCompilerFlag)
if(UNIX)
  CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
  if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_STANDARD 17) # C++14...
    set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
    set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
  endif()
elseif(MSVC) # Use Visual Studio as a compiler which comes with C++17!
  CHECK_CXX_COMPILER_FLAG("/std:c++17" COMPILER_SUPPORTS_CXX17)
  if (COMPILER_SUPPORTS_CXX17)
    add_compile_options("/std:c++17")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG")
endif()
add_compile_definitions("_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING") # To supress C4996 warning

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

if(MSVC)
  set(WIN32_FLAG WIN32)
endif()
######## ######## ######## ######## ######## ######## ######## ########


######## ######## ######## ######## ######## ######## ######## ########
# Adding executable
######## ######## ######## ######## ######## ######## ######## ########
add_executable(${PROJECT_NAME} ${WIN32_FLAG}
        include/islay/Worker.cpp
        src/main.cpp
        src/Application.cpp
        src/Engine.cpp
        src/WorkerSample.cpp
)
######## ######## ######## ######## ######## ######## ######## ########


######## ######## ######## ######## ######## ######## ######## ########
# Linking libraries
######## ######## ######## ######## ######## ######## ######## ########
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

## Threads
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

## OpenCV
find_package(OpenCV REQUIRED HINTS ${OpenCV_DIR})
set(OpenCV_DIR "" CACHE PATH "OpenCV install path")
#find_package(OpenCV CONFIG REQUIRED) # by vcpkg
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

## Eigen
find_package(Eigen3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

## rapidjson
target_include_directories( ${PROJECT_NAME} PRIVATE 3rdparty/rapidjson/include)

## spdlog
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/spdlog/include)

## Dear Imgui & implot
find_package(SDL2 REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_library(imgui SHARED
        # imgui
        3rdparty/imgui/imgui.cpp
        3rdparty/imgui/imgui_demo.cpp
        3rdparty/imgui/imgui_draw.cpp
        3rdparty/imgui/imgui_widgets.cpp
        3rdparty/imgui/examples/imgui_impl_sdl.cpp
        3rdparty/imgui/examples/imgui_impl_opengl3.cpp
        3rdparty/imgui/examples/libs/gl3w/GL/gl3w.c
        # implot
        3rdparty/implot/implot.h
        3rdparty/implot/implot_internal.h
        3rdparty/implot/implot.cpp
        3rdparty/implot/implot_items.cpp
)
set_source_files_properties(imgui/examples/libs/gl3w/GL/gl3w.c PROPERTIES COMPILE_FLAGS -Wno-pedantic)

target_include_directories(imgui PUBLIC
  3rdparty/imgui
  3rdparty/imgui/examples
  3rdparty/imgui/examples/libs/gl3w
  ${SDL2_INCLUDE_DIRS}
  3rdparty/implot
)
if(UNIX AND NOT APPLE)
  target_link_libraries(imgui PUBLIC ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} GLEW::GLEW GL dl )
else(APPLE)
  target_link_libraries(imgui PUBLIC ${SDL2_LIBRARIES} ${GLFW_LIBRARIES} GLEW::GLEW dl)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

## hwloc
# Currenlty hwloc does not support find_package, but is planned to supprot according to this PR (https://github.com/open-mpi/hwloc/pull/566). Stay tuned to the PR getting merged to the official release.
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  unset(HWLOC_FOUND CACHE)
  pkg_search_module(HWLOC hwloc)
  if(HWLOC_FOUND)
    message(STATUS "FOUND hwloc. ${HWLOC_VERSION}")
    message(STATUS "\tHWLOC_LIBRARIES:${HWLOC_LIBRARIES}")
    message(STATUS "\tHWLOC_LINK_LIBRARIES:${HWLOC_LINK_LIBRARIES}")
    message(STATUS "\tHWLOC_LIBRARY_DIRS:${HWLOC_LIBRARY_DIRS}")
    message(STATUS "\tHWLOC_LDFLAGS:${HWLOC_LDFLAGS}")
    message(STATUS "\tHWLOC_LDFLAGS_OTHERS:${HWLOC_LDFLAGS_OTHERS}")
    message(STATUS "\tHWLOC_INCLUDE_DIRS:${HWLOC_INCLUDE_DIRS}")
    message(STATUS "\tHWLOC_CFLAGS:${HWLOC_CFLAGS}")
    message(STATUS "\tHWLOC_CFLAGS_OTHER:${HWLOC_CFLAGS_OTHER}")
  else()
    message(FATAL_ERROR "HWLOC not found with pkg-config, add the path to hwloc.pc in PKG_CONFIG_PATH.")
  endif()
else()
  message(FATAL_ERROR "PKG_CONFIG_EXECUTABLE: not found.")
endif()

add_library(hwloc INTERFACE)
target_include_directories(hwloc INTERFACE ${HWLOC_INCLUDE_DIRS})
target_compile_options(hwloc INTERFACE ${HWLOC_CFLAGS})
target_link_libraries(hwloc INTERFACE ${HWLOC_LINK_LIBRARIES})
target_link_options(hwloc INTERFACE ${HWLOC_LDFLAGS})

target_link_libraries(${PROJECT_NAME} PRIVATE hwloc)
######## ######## ######## ######## ######## ######## ######## ########

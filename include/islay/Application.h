//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_APPLICATION_H
#define ISLAY_APPLICATION_H

// dear imgui: standalone example application for SDL2 + OpenGL
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_apps.h"
#include <cstdio>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <string>
#include <opencv2/opencv.hpp>

class Application {
    SDL_Window* window;
    SDL_GLContext gl_context;
    std::string glsl_version;

public:
    Application();
    ~Application();

    bool run();

};



#endif //ISLAY_APPLICATION_H

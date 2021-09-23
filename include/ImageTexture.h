//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_IMAGETEXTURE_H
#define ISLAY_IMAGETEXTURE_H

//#include <cstdio>
//#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include <SDL.h>

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Credit: https://github.com/ashitani/opencv_imgui_viewer
class ImageTexture {
private:
    int width, height;
    GLuint my_opengl_texture;

public:
    ~ImageTexture(){
        glBindTexture(GL_TEXTURE_2D, 0);  // unbind texture
        glDeleteTextures(1, &my_opengl_texture);
    };

    void setImage(cv::Mat *pframe, float mag = 1.0){ // from cv::Mat (BGR)

        if(!pframe->empty()) {
            resize(*pframe, *pframe, cv::Size(), mag, mag, cv::INTER_NEAREST);

            width = pframe->cols;
            height = pframe->rows;

            glGenTextures(1, &my_opengl_texture);
            glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            if (pframe->channels() == 3) { // Pretty ugly, costly implementation...
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
                             GL_UNSIGNED_BYTE, (pframe->data));
            } else if (pframe->channels() == 1 && pframe->depth() == CV_8U) {
                cv::cvtColor(*pframe, *pframe, cv::COLOR_GRAY2BGR);
                // Some enviromnent doesn't support GP_BGR
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
                             GL_UNSIGNED_BYTE, (pframe->data));
            } else if (pframe->channels() == 1 && pframe->depth() == CV_32F) {
                cv::cvtColor(*pframe, *pframe, cv::COLOR_GRAY2BGR);
                // Some enviromnent doesn't support GP_BGR
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
                             GL_FLOAT, (pframe->data));
            }
        }
    };

    void getOpenCVMat(); /// TODO: implement this. Get OpenCV Mat from OpenGL texture

    void setImage(std::string filename){ // from file
        cv::Mat frame = cv::imread(filename);
        setImage(&frame);
    };

    void* getOpenglTexture(){
        return (void*)(intptr_t)my_opengl_texture;
    };

    ImVec2 getSize(){ return ImVec2(width, height); };
};

#endif //ISLAY_IMAGETEXTURE_H

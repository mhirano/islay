//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "AppMsg.h"

bool EngineOffline::run() {
    worker = std::thread([this] {
        /**
         * Main process
         * - You write your algorithm here.
         */
        cv::Mat lena = cv::imread("../res/lena.png");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        cv::Mat canvas1, canvas2;
        int k = ceil(rand()%5)*8+1;
        cv::GaussianBlur( lena, canvas1, cv::Size(k,k), 0 );
        cv::GaussianBlur( lena, canvas2, cv::Size(k*2+1,k*2+1), 0 );

        /**
         * Show processed image
         * - You can show img(cv::Mat) by adding
         *     dm->pool["Name of window"] = img;
         * - You can modify ImageTexture::setImage to show a Mat which is not a currently supported format.
         */
        DispMsg *dm;
        dm = appMsg->displayMessenger->prepareMsg();
        dm->pool["original lena"] = lena;
        dm->pool["blurred lena"] = canvas1;
        dm->pool["doubly blurred lena"] = canvas2;

        appMsg->displayMessenger->send();
        if (appMsg->displayMessenger->isClosed()) {
//            break;
        }
    });

    std::cout << "Image processing for a single frame done ..." << std::endl;

    reset();

    return true;
}

bool EngineOffline::reset() {
    if (worker.joinable()) {
        worker.join();
    }
    return true;
}
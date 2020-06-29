//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "AppMsg.h"
#include "Config.h"

bool EngineOffline::run() {
    Config::get_instance().saveConfig();

    worker = std::thread([this] {
        /**
         * Main process
         * - You write your algorithm here.
         */
        cv::Mat lena(Config::get_instance().readIntParam("IMAGE_WIDTH"),
                     Config::get_instance().readIntParam("IMAGE_HEIGHT"), CV_8UC3);
        lena = cv::imread(
                Config::get_instance().resourceDirectory() + "/" +
                Config::get_instance().readStringParam("IMG_PATH"));
        std::string imgName = Config::get_instance().readStringParam("IMG_NAME");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        cv::Mat blurred_lena;
        int k = ceil(rand()%5)*8+1;
        cv::GaussianBlur( lena, blurred_lena, cv::Size(k,k), 0 );

        cv::imwrite(Config::get_instance().resultDirectory() + Config::get_instance().readStringParam("BLURRED_IMG"),
                    blurred_lena, std::vector<int>{cv::IMWRITE_PNG_COMPRESSION});

        /**
         * Show processed image
         * - You can show cv::Mat img by simply adding
         *     dm->pool["Name of window"] = img;
         * - Images show on the same location by default.
         * - You can modify ImageTexture::setImage to show a Mat which is not a currently supported format.
         */
        DispMsg *dm;
        dm = appMsg->displayMessenger->prepareMsg();
        dm->pool[imgName.c_str()] = lena;
        dm->pool["blurred lena"] = blurred_lena;
//        dm->pool["resized"] = resized;

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
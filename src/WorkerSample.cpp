//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "WorkerSample.h"

#include <opencv2/opencv.hpp>
#include <islay/Utility.h>

bool WorkerSample::run(const std::shared_ptr<void> data) {
    SPDLOG_DEBUG("Calling WorkerSample::run");
    for (int i = 1; i <= 5; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        SPDLOG_DEBUG("\tprocessing for {} second", i);
        if (checkIfTerminateRequested()) {
            break;
        }
    }
    return true;
}

bool WorkerSampleWithAppMsg::run(const std::shared_ptr<void> data){
    /*
     * Save config file as of run experiment
     */
    Config::get_instance().saveConfig();

    /*
     * Retrieve data passed by user
     */
    std::shared_ptr<int> i = (std::static_pointer_cast<int>) (data);
    SPDLOG_DEBUG("Received value from EngineOffline::runTest: {}", *i);

    /**
     * Main process
     * - You write your algorithm here.
     * - You can access to config parameters via Config::get_instance().readXYZParam("PARAM"); (set functions are not thread safe for now)
     */
    cv::Mat lena(Config::get_instance().readIntParam("IMAGE_WIDTH"),
                 Config::get_instance().readIntParam("IMAGE_HEIGHT"), CV_8UC3);
    lena = cv::imread(
            Config::get_instance().resourceDirectory() + "/" +
            Config::get_instance().readStringParam("IMG_PATH"));
    std::string imgName = Config::get_instance().readStringParam("IMG_NAME");

    auto elapsedTimeInMs = Util::Bench::bench([&] {
        for (int i = 0; i < 3000; i++) {
            cv::Mat blurred_lena;
            int k = ceil(rand() % 5) * 8 + 1;
            cv::GaussianBlur(lena, blurred_lena, cv::Size(k, k), 10);

            /**
             * Show processed image
             * - You can show cv::Mat img by simply adding
             *     dm->pool["Name of window"] = img;
             *   between prepareMsg() and send().
             * - Images show up at the same location by default.
             * - You can modify ImageTexture::setImage to show a cv::Mat whose format is not currently supported.
             */
            DispMsg *dm;
            dm = appMsg->displayMessenger->prepareMsg();
            dm->pool[imgName.c_str()] = lena;
            dm->pool["blurred lena"] = std::move(blurred_lena);
            appMsg->displayMessenger->send();

            if (checkIfTerminateRequested()) {
                break;
            }
        }
    });

    SPDLOG_INFO("WorkerSampleWithAppMsg took {}ms", elapsedTimeInMs);

    return true;
}
//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "WorkerSample.h"

#include <opencv2/opencv.hpp>
#include <islay/Utility.h>

bool WorkerSample::run(const std::shared_ptr<void> data) {
    SPDLOG_DEBUG("Calling WorkerSample::run");

    /*
     * Save config file as of run experiment
     */
    Config::get_instance().saveConfig();

    /*
     * Retrieve data passed by user
     */
    std::shared_ptr<int> i = (std::static_pointer_cast<int>) (data);
    SPDLOG_DEBUG("Received value from EngineOffline::runTest: {}", *i);

    /*
     * Main process - Write your algorithm below.
     */

    // You can access to config parameters via Config::get_instance().readXYZParam("PARAM"); (set functions are not thread safe for now)
    cv::Mat lena(Config::get_instance().readIntParam("IMAGE_WIDTH"),
                 Config::get_instance().readIntParam("IMAGE_HEIGHT"), CV_8UC3);

    // Read image from a file specified by the config file
    lena = cv::imread(
            Config::get_instance().resourceDirectory() + "/" +
            Config::get_instance().readStringParam("IMG_PATH"));

    // Retrieve image name from the config.
    std::string imgName = Config::get_instance().readStringParam("IMG_NAME");

    for (int i = 0; i < 3; i++) {
        cv::Mat blurred_lena;
        int k = ceil(rand() % 5) * 8 + 1;
        cv::GaussianBlur(lena, blurred_lena, cv::Size(k, k), 10);

        /*
         * Save blurred lena in the result directory
         *  - Result files for each execution are stored at a result directory under `result` directory.
         */
        cv::imwrite(Config::get_instance().resultDirectory() + "/result_" + std::to_string(i) + ".png", blurred_lena,
                    {cv::IMWRITE_PNG_COMPRESSION});

        /*
         * Be careful in drawing images
         *  - Codes here are executed in a user-created thread, not the main thread.
         *    This means you cannot draw blurred lena using cv::imshow() here.
         *    Instead, to draw an image within this function, use AppMsg as in the next example.
         */
//        cv::imshow("test", blurred_lena); // THIS CAUSES ERROR.

        if (checkIfTerminateRequested()) {
            break;
        }
    }

    SPDLOG_INFO("WorkerSample::run completed");

    return true;
}

bool WorkerSampleWithAppMsg::run(const std::shared_ptr<void> data){
    SPDLOG_DEBUG("Calling WorkerSampleWithAppMsg::run");

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
     * - Write your algorithm here.
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
             * Show processed image using AppMsg
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
//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "AppMsg.h"
#include "Config.h"
#include "Logger.h"

namespace Bench {
    template <typename TimeT = std::chrono::milliseconds, typename F>
    inline TimeT take_time(F &&f) {
        const auto begin = std::chrono::high_resolution_clock::now();
        f();
        const auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<TimeT>(end - begin);
    }

    template<typename TimeT, typename F> struct BenchDelegate {
        static long delegatedBenchFunc(F&& f){
            const auto t = take_time<TimeT>(std::forward<F>(f));
            std::chrono::duration<long, std::milli> t_ = t;
            printf("%ld [ms]\n", t_.count());
            SPDLOG_INFO("{} [ms]", t_.count());
            return t_.count();
        }
    };

    template <typename TimeT = std::chrono::milliseconds, typename F>
    inline long bench(F &&f) {
        return BenchDelegate<TimeT, F>::delegatedBenchFunc(std::forward<F>(f));
    }

}


bool EngineOffline::run() {

    worker = std::thread([this] {
        workerStatus.store(WORKER_STATUS::RUNNING);
        /*
         * Save config file as of run experiment
         */
        Config::get_instance().saveConfig();

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

        cv::Mat blurred_lena;
        Bench::bench([&] {
            for (int i = 0; i < 10000; i++) {
                int k = ceil(rand() % 5) * 8 + 1;
                cv::GaussianBlur(lena, blurred_lena, cv::Size(k, k), 0);
            }
        });
        cv::imwrite(Config::get_instance().resultDirectory() + Config::get_instance().readStringParam("BLURRED_IMG"),
                    blurred_lena);


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
        dm->pool["blurred lena"] = blurred_lena;

        appMsg->displayMessenger->send();
        if (appMsg->displayMessenger->isClosed()) {
//            break;
        }
        workerStatus.store(WORKER_STATUS::IDLE);
    });

    SPDLOG_DEBUG("Image processing for a single frame done ...");

//    reset();

    return true;
}

bool EngineOffline::reset() {
    if (worker.joinable()) {
        worker.join();
    }
    return true;
}
//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "WorkerSample.h"

#include <opencv2/opencv.hpp>
#include <islay/Utility.h>
#include <hwloc.h>

bool WorkerSample::run(const std::shared_ptr<void> data){
    SPDLOG_DEBUG("Calling WorkerSample::run");

    /**
     * Save config file as of run experiment
     */
    Config::get_instance().saveConfig();

    /**
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

    /**
     * Be careful! You can't imshow in a worker.
     * - Codes here are executed in a user-created thread, not the main thread.
     *   This means you cannot draw blurred lena using cv::imshow() here.
     */
    // cv::imshow("test", lena); // THIS CAUSES ERROR.

    /**
     * You can save a cv::Mat though. Saving blurred lena in the result directory
     * - Result files for each execution are stored in a result directory under `result` directory.
     */
    cv::imwrite(Config::get_instance().resultDirectory() + "/lena_imwrite.png", lena );

    /**
     * Show processed image using AppMsg
     * - Images show up at the same location by default.
     * - You can modify ImageTexture::setImage to show a cv::Mat whose format is not currently supported.
     */
    auto msgr = appMsg->ocvImageMsgCollection.setup("lena"); // Set up a messenger with a name
    auto msg = msgr->prepareMsg(); // Prepare a message
    msg->img = lena; // pass an image you want to show to the message
    msgr->send(); // Send the messenger

    /**
     * You can measure elapsed time using Util::Bench::bench
     */
    auto elapsedTimeInMs = Util::Bench::bench([&] {
        for (int i = 0; i < 3000; i++) {
            cv::Mat blurred_lena;
            int k = ceil(rand() % 5) * 8 + 1;
            cv::GaussianBlur(lena, blurred_lena, cv::Size(k, k), 10);

            auto msgr = appMsg->ocvImageMsgCollection.setup("lena_blur"); // make sure to set up each time
            auto msg = msgr->prepareMsg();
            msg->img = std::move(blurred_lena);
            msgr->send();

            if (checkIfTerminateRequested()) {
                break;
            }
        }
    });

    SPDLOG_INFO("WorkerSample took {}ms", elapsedTimeInMs);

    return true;
}

bool WorkerSampleWithCpuBinding::run(const std::shared_ptr<void> data) {

// @brief 現在利用されている論理プロセッサ番号,物理プロセッサ番号を表示する
// @param[in] t トポロジーコンテキスト
    auto PrintUsingProccessor = [](hwloc_topology_t t) {
        {
            constexpr int N = 30;
            hwloc_bitmap_t b = hwloc_bitmap_alloc();
            hwloc_obj_t pu, core;
            for (int i = 0; i < N; ++i) {
                hwloc_get_last_cpu_location(t, b, HWLOC_CPUBIND_THREAD);
                pu = hwloc_get_pu_obj_by_os_index(t, hwloc_bitmap_first(b));
                core = pu->parent;
                std::stringstream os;
                os << "Thread_id=" << std::this_thread::get_id() << ", "
                     << "Processor=" << pu->logical_index
                     << "(" << core->logical_index << ")";
                SPDLOG_INFO("{}", os.str());
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            hwloc_bitmap_free(b);
        }
    };

// @brief 指定されたプロセッサ番号にスレッドを固定する
// @param[in] t               トポロジーコンテキスト
// @param[in] processorNumber プロセッサ番号
    auto BindProcessor = [](hwloc_topology_t t, int processorNumber)
    {
        const int depthTop = hwloc_topology_get_depth(t);
        const hwloc_obj_t pu =
                hwloc_get_obj_by_depth(t, depthTop - 1, processorNumber);
        SPDLOG_INFO("pu type: {}", pu->type);
        if(pu->type == hwloc_obj_type_t::HWLOC_OBJ_PU) SPDLOG_INFO("This is PU");
        hwloc_set_cpubind(t, pu->cpuset, HWLOC_CPUBIND_THREAD);
    };

    constexpr int processorNumber = 0;
    hwloc_topology_t t;
    hwloc_topology_init(&t);
    hwloc_topology_load(t);
    SPDLOG_INFO("# Default");
    PrintUsingProccessor(t);
//    BindProcessor(t,processorNumber);
//    SPDLOG_INFO("# Bind");
//    PrintUsingProccessor(t);
//    hwloc_topology_destroy(t);

    return true;
}
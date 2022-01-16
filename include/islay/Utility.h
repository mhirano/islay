//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_UTILITY_H
#define ISLAY_UTILITY_H

#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>

namespace Util {

    /**
     * Show std::string on given cv::Mat
     */
    enum class TEXT_POS { LT = 0, /* left top */ RT = 1, /* right top */ RB = 2 /* right bottom */};
    inline void putTextBG(cv::Mat &img, const std::string &label, TEXT_POS pos=TEXT_POS::LT) {

        double fontSizeRatio = 1.5; // tweek this to change font size

        cv::Size imgSize = img.size();
        int largerEdgeLength = std::max<int>(imgSize.width, imgSize.height);

        double labelFontScale = largerEdgeLength / 1600.0 * fontSizeRatio;
        int labelThickness = ceil(largerEdgeLength / 1600.0 * fontSizeRatio);

        int baseLine;
        cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                         labelFontScale, labelThickness, &baseLine);
        switch (pos) {
            case TEXT_POS::LT: {
                cv::Point leftTop(40, 20);
                int top = leftTop.y, left = leftTop.x;
                cv::rectangle(img, cv::Point(left, top),
                              cv::Point(left + round(labelSize.width), top + round(labelSize.height) + baseLine),
                              cv::Scalar(255, 255, 255), cv::FILLED);
                cv::putText(img, label, cv::Point(left, top + round(labelSize.height) + round(baseLine / 2.0)),
                            cv::FONT_HERSHEY_SIMPLEX, labelFontScale, cv::Scalar(0, 0, 0), labelThickness);
                break;
            }
            case TEXT_POS::RT: {
                cv::Point rightTop(imgSize.width - 40, 20);
                int top = rightTop.y, right = rightTop.x;
                cv::rectangle(img, cv::Point(right - round(labelSize.width), top),
                              cv::Point(right, top + round(labelSize.height) + baseLine), cv::Scalar(255, 255, 255),
                              cv::FILLED);
                cv::putText(img, label, cv::Point(right - round(labelSize.width),
                                                  top + round(labelSize.height) + round(baseLine / 2.0)),
                            cv::FONT_HERSHEY_SIMPLEX, labelFontScale, cv::Scalar(0, 0, 0), labelThickness);
                break;
            }
            case TEXT_POS::RB: {
                cv::Point rightBottom(imgSize.width - 40, imgSize.height - 20);
                int bottom = rightBottom.y, right = rightBottom.x;
                cv::rectangle(img,
                              cv::Point(right - round(labelSize.width), bottom - round(labelSize.height) - baseLine),
                              cv::Point(right, bottom), cv::Scalar(255, 255, 255), cv::FILLED);
                cv::putText(img, label, cv::Point(right - round(labelSize.width), bottom - round(baseLine / 2.0)),
                            cv::FONT_HERSHEY_SIMPLEX, labelFontScale, cv::Scalar(0, 0, 0), labelThickness);
                break;
            }
            default: {
                std::cerr << "Invalid text position" << std::endl;
                break;
            }
        }
    }

    inline std::string now(){
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto tm_now = std::localtime(&now);
        std::ostringstream os;
        os.str("");
        os.clear();
        os << (tm_now->tm_year+1900)
           << std::setw(2) << std::setfill('0') << (tm_now->tm_mon+1)
           << std::setw(2) << std::setfill('0') << tm_now->tm_mday << "-"
           << std::setw(2) << std::setfill('0') << tm_now->tm_hour
           << std::setw(2) << std::setfill('0') << tm_now->tm_min
           << std::setw(2) << std::setfill('0') << tm_now->tm_sec;
        return os.str();
    }

    /* How to use:
        Bench::bench([&]{
            // process to bench
            // outputs elapsed time in milliseconds, like 30 [ms]
        });

        Bench::bench<std::chrono::milliseconds>([&]{
            // process to bench
            // outputs elapsed time in milliseconds, like 40 [ms]
        });

        Bench::bench<std::chrono::microseconds>([&]{
            // process to bench
            // outputs elapsed time in microseconds, like 50 [us]
            // Note: In VC2015+, std::chrono::high_resolution_clock is equivalent to QueryPerformanceCounter
        });

        Overhead of bench is at most 20 us in MacPro Late 2013 (3.7GHz Quad-Core Intel Xeon E5)
     */
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
                SPDLOG_INFO("{} [ms]", t_.count());
                return t_.count();
            }
        };

        template <typename TimeT = std::chrono::milliseconds, typename F>
        inline long bench(F &&f) {
            return BenchDelegate<TimeT, F>::delegatedBenchFunc(std::forward<F>(f));
        }
    }
}

#endif //ISLAY_UTILITY_H

//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_UTILITY_H
#define ISLAY_UTILITY_H

//#include <time.h>
#include <iostream>
//#include <vector>
//#include <numeric>
//#include <fstream>
//#include <cmath>
#include <chrono>
//#include <sstream>

#include <opencv2/opencv.hpp>

//#define PI 3.14159265358979323846

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
}

#endif //ISLAY_UTILITY_H

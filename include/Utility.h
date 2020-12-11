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

    inline void putTextBG(cv::Mat &img, std::string label, cv::Point lt = cv::Point(40, 40), double fontScale = 0.5) {

        int baseLine;
        cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, fontScale, 1, &baseLine);
        int top = lt.y, left = lt.x;
        cv::rectangle(img, cv::Point(left, top - round(1.5*labelSize.height)),
                      cv::Point(left + round(1.5*labelSize.width), top + baseLine), cv::Scalar(255, 255, 255),
                      cv::FILLED);
        cv::putText(img, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, fontScale*1.5, cv::Scalar(0, 0, 0), 1);
    }

    inline std::string now(){
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string s(15, '\0');
        std::strftime(&s[0], s.size(), "%Y%m%d-%H%M%S", std::localtime(&now));
        return s;
    }
}

#endif //ISLAY_UTILITY_H

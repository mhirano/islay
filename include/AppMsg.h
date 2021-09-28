//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_APPMSG_H
#define ISLAY_APPMSG_H

#include <opencv2/opencv.hpp>
#include <map>
#include <memory>
#include "InterThreadMessenger.hpp"

struct DispMsg : public MsgData {
    std::map<std::string, cv::Mat> pool;
};

class AppMsg{
public:
    AppMsg():displayMessenger(new InterThreadMessenger<DispMsg>){};
    InterThreadMessenger<DispMsg>* displayMessenger;

    void close(){
        displayMessenger->close();
    };
};

using AppMsgPtr = std::shared_ptr<AppMsg>;

#endif //ISLAY_APPMSG_H

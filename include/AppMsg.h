//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_APPMSG_H
#define ISLAY_APPMSG_H

#include <opencv2/opencv.hpp>
#include <map>
#include "islay/InterThreadMessenger.hpp"

struct OcvImageMsg : public MsgData {
    cv::Mat img;
};

struct OcvImageMessengerCollection{
    std::map<std::string, std::shared_ptr<InterThreadMessenger<OcvImageMsg>>> pool;

    std::shared_ptr<InterThreadMessenger<OcvImageMsg>> setup(std::string name){
        bool hasMsg;
        pool.count(name) == 0? hasMsg=false: hasMsg=true;
        if(!hasMsg)
            pool[name] = std::make_shared<InterThreadMessenger<OcvImageMsg>>();
        return pool[name];
    }

    void clear(){
        pool.clear();
    }

    bool close(){
        for (auto& msg: pool){ msg.second->close(); }
        return true;
    }
};

class AppMsg{
public:
    /**
     * Image Collection
     */
    OcvImageMessengerCollection ocvImageMsgCollection;

    void close(){
        ocvImageMsgCollection.close();
    };
};

using AppMsgPtr = std::shared_ptr<AppMsg>;

#endif //ISLAY_APPMSG_H

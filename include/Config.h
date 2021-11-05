//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_CONFIG_H
#define ISLAY_CONFIG_H

#include <filesystem>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/prettywriter.h>

#include <Eigen/Core>
#include <string>

class Config {
private:
    Config() {
        {
            FILE *fp;
            char buf[512];

            // read
#ifdef _MSC_VER
			fp = fopen("../config/config_win_default.json", "rb");
#else
			fp = fopen("config_default.json", "rb");
#endif
            rapidjson::FileReadStream rs(fp, buf, sizeof(buf));

            config.ParseStream<rapidjson::ParseFlag::kParseCommentsFlag>(rs);

            fclose(fp);
        }

        createResultDirectory();
    };

    ~Config() = default;

    void createResultDirectory() {
        std::string currentDateAndTime;
        {
            time_t now = time(0);
            struct tm tstruct;
            tstruct = *localtime(&now);
            std::ostringstream os;
            os.clear();
            os.str("");
            os << std::put_time(&tstruct, "%Y%m%d-%H%M%S");
            currentDateAndTime = os.str();
        }

        const char *resultParentDirectoryChar = config["RESULT_PARENT_DIRECTORY"].GetString();
        std::string resultParentDirectory_(resultParentDirectoryChar);
        std::string resultDirName = resultParentDirectory_ + "/" + currentDateAndTime;

		std::filesystem::path path(resultDirName);
        std::error_code error;
        const bool result = std::filesystem::create_directory(path, error);
        if (!result || error) {
            std::cerr << "Failed in creating result directory" << std::endl;
        }

        config.AddMember("RESULT_DIRECTORY", rapidjson::Value(resultDirName.c_str(), config.GetAllocator()), config.GetAllocator());
    }

    rapidjson::Document config;

public:
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    Config(Config &&) = delete;
    Config &operator=(Config &&) = delete;

    static Config &get_instance() {
        static Config instance;
        return instance;
    }

    const rapidjson::Document &getDocument() const { return config; }

    void saveConfig() {
        FILE *fp = fopen(( this->resultDirectory() + "/config.json").c_str(), "wb"); // non-Windows use "w"

        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        config.Accept(writer);

        fclose(fp);
    }

    void saveConfig(std::string name) {
        FILE *fp = fopen((this->resultDirectory() + "/" + name).c_str(), "wb"); // non-Windows use "w"

        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        config.Accept(writer);

        fclose(fp);
    }

    void loadConfig(std::string configFileName) {
        {
            FILE *fp;
            char buf[512];

            fp = fopen((configFileName).c_str(), "rb");
            if (fp == NULL) {
                std::cerr << "Failed to load " << configFileName << std::endl;
            }
            rapidjson::FileReadStream rs(fp, buf, sizeof(buf));

            config.ParseStream<rapidjson::ParseFlag::kParseCommentsFlag>(rs);

            fclose(fp);
        };

        saveConfig();
    }

    std::string showConfig() {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        config.Accept(writer);
        return std::string(buffer.GetString());
    }

    std::string resultDirectory(){
        return std::string(config["RESULT_DIRECTORY"].GetString());
    }

    std::string resourceDirectory(){
        return std::string(config["RESOURCE_DIRECTORY"].GetString());
    }

    double readDoubleParam(std::string paramName) const {
        return config[paramName.c_str()].GetDouble();
    }

    int readIntParam(std::string paramName) const {
        return config[paramName.c_str()].GetInt();
    }

    std::string readStringParam(std::string paramName) const {
        return std::string(config[paramName.c_str()].GetString());
    }

    bool readBoolParam(std::string paramName) const {
        return config[paramName.c_str()].GetBool();
    }

    Eigen::VectorXd readVectorParam(std::string paramName) const {
        auto array = config[paramName.c_str()].GetArray();
        int num = array.Size();

        Eigen::VectorXd vector(num);
        for (int i = 0; i < num; i++) {
            vector(i) = array[i].GetDouble();
        }
        return vector;
    }

    Eigen::MatrixXd readMatrixParam(std::string paramName) const {
        auto array = config[paramName.c_str()].GetArray();

        Eigen::MatrixXd matrix;
        if(array[0].IsArray()) { // [Row x COl] or [Row x 1]
            matrix = Eigen::MatrixXd(array.Size(),array[0].Size());
        } else { // [1 x Col] or [1 x 1]
            matrix = Eigen::MatrixXd(1, array.Size());
        }

        std::cout << matrix.rows() << "x" << matrix.cols() << std::endl;

        return matrix;
    }

    bool setStringParam(std::string paramName, std::string paramToSet) {
        if (config[paramName.c_str()].IsString()) {
            config[paramName.c_str()].SetString(paramToSet.c_str(), paramToSet.length(), config.GetAllocator());
            return true;
        } else {
            std::cerr << "Invalid config parameter setting" << std::endl;
            return false;
        };
    }

    bool setDoubleParam(std::string paramName, double paramToSet) {
        if (config[paramName.c_str()].IsDouble()) {
            config[paramName.c_str()].SetDouble(paramToSet);
            return true;
        } else {
            std::cerr << "Invalid config parameter setting" << std::endl;
            return false;
        }
    }
};


#endif //ISLAY_CONFIG_H

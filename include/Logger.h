//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_LOGGER_H
#define ISLAY_LOGGER_H

#include <iostream>

#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO // All DEBUG/TRACE statements will be removed by the pre-processor
#endif
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/stdout_color_sinks.h" // or "../stdout_sinks.h" if no colors needed
#include "spdlog/sinks/ostream_sink.h"

class Logger
{
private:
    Logger(){
        try
        {   // Default logger
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
            auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt> (oss);
            spdlog::sinks_init_list sink_list = { file_sink, console_sink, ostream_sink};
            for (auto &sink:sink_list) {
                sink->set_pattern("[%C-%m-%d %H:%M:%S.%f][%^%5l%$] %v");
            }
            logger.reset(new spdlog::logger("LOGGER", sink_list.begin(), sink_list.end()));
            logger->set_level(spdlog::level::trace);
            spdlog::set_default_logger(logger);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cout << "[Log initialization failed] " << ex.what() << std::endl;
        }
    };
    ~Logger() = default;

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& get_instance()
    {
        static Logger instance;
        return instance;
    }

    void setExportDirectory(std::string logExportDirectory){
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logExportDirectory + "/log.txt", true);
        auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt> (oss);
        spdlog::sinks_init_list sink_list = { file_sink, console_sink, ostream_sink};
        for (auto &sink:sink_list) {
            sink->set_pattern("[%C-%m-%d %H:%M:%S.%f][%^%5l%$] %v");
        }
        logger.reset(new spdlog::logger("LOGGER", sink_list.begin(), sink_list.end()));
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
    }

    std::shared_ptr<spdlog::logger> logger;
    std::ostringstream oss;
};

#endif //ISLAY_LOGGER_H

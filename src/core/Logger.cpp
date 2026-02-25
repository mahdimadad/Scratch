#include "core/Logger.h"
#include <iostream>
int Logger::currentCycle = 0;

void Logger::init() {
    currentCycle = 0;
}

void Logger::setCycle(int cycle) {
    currentCycle = cycle;
}

void Logger::log(LogLevel level,
                 const std::string &command,
                 const std::string &message) {
    std::string levelStr;
    switch (level) {
        case LOG_INFO: levelStr = "INFO";
            break;
        case LOG_WARNING: levelStr = "WARNING";
            break;
        case LOG_ERROR: levelStr = "ERROR";
            break;
    }
    std::cout
            << "[Cycle:" << currentCycle << "] "
            << "[" << levelStr << "] "
            << "[" << command << "] "
            << message
            << std::endl;
}

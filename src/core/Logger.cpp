#include "core/Logger.h"
#include <iostream>
using namespace std;
void Logger::log(int cycle, const string &command, const string &message, LogLevel level) {
    string levelStr;
    switch (level) {
        case LogLevel::INFO: levelStr = "INFO";
            break;
        case LogLevel::WARNING: levelStr = "WARNING";
            break;
        case LogLevel::ERROR: levelStr = "ERROR";
            break;
    }
    cout << "[Cycle:" << cycle << "] " << "[" << levelStr << "] " << "[" << command << "] " << message << endl;
}
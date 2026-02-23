#include "core/Logger.h"
#include <iostream>
using namespace std;
void logMessage(int cycle, const string &command, const string &message, LogLevel level) {
    string levelStr;
    switch (level) {
        case INFO: levelStr = "INFO";
            break;
        case WARNING: levelStr = "WARNING";
            break;
        case ERROR: levelStr = "ERROR";
            break;
        default: levelStr = "INFO";
            break;
    }
    cout << "[Cycle:" << cycle << "] " << "[" << levelStr << "] " << "[" << command << "] " << message << endl;
}
#ifndef LOGGER_H
#define LOGGER_H
#include <string>
enum LogLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};
class Logger {
public:
    static void init();
    static void setCycle(int cycle);
    static void log(LogLevel level,
                    const std::string &command,
                    const std::string &message);
private:
    static int currentCycle;
};
#endif
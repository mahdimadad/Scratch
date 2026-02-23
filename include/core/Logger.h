#ifndef LOGGER_H
#define LOGGER_H
#include <string>
using namespace std;
enum class LogLevel { INFO, WARNING, ERROR };
class Logger {
public:
    static void log(int cycle, const string &command, const string &message, LogLevel level = LogLevel::INFO);
};
#endif
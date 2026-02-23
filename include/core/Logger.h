#ifndef LOGGER_H
#define LOGGER_H
#include <string>
using namespace std;
enum LogLevel { INFO, WARNING, ERROR };
void logMessage(int cycle, const string &command, const string &message, LogLevel level = INFO);
#endif
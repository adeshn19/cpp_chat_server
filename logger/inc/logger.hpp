#pragma once
#include <string>

enum class LogLevel { INFO, DEBUG, ERROR };

void logMessage(LogLevel level, const std::string &msg);
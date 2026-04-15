#include "logger.hpp"
#include "utils.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
std::mutex logMutex;

std::string levelToString(LogLevel level) {
  switch (level) {
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::ERROR:
    return "ERROR";
  }
  return "UNKNOWN";
}

void logMessage(LogLevel level, const std::string &msg) {

  if (level == LogLevel::DEBUG) {
    return;
  }
  std::lock_guard<std::mutex> lock(logMutex);

  std::string logLine =
      "[" + getCurrentTime() + "] [" + levelToString(level) + "] " + msg;

  std::ofstream file("../logs/server.log", std::ios::app);
  file << logLine << std::endl;

  std::cout << logLine << std::endl;
}
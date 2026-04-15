#include "utils.hpp"
#include <ctime>

std::string getCurrentTime() {
  time_t now = time(0);
  char *dt = ctime(&now);

  std::string timeStr(dt);
  timeStr.pop_back(); // remove newline

  return timeStr;
}
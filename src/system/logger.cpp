#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::_instance;

/*static*/ void Logger::init() {
  if (_instance == nullptr) {
    _instance = spdlog::stdout_color_mt("Tetris");
    _instance->set_pattern("[%D %T] %^[%l]%$ %v");
  }
}

/*static*/ std::shared_ptr<spdlog::logger>& Logger::get() {
  return _instance;
}

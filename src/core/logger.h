#pragma once

#include <spdlog/spdlog.h>

class Logger {
public:
  static void init();
  static std::shared_ptr<spdlog::logger>& get();

private:
  static std::shared_ptr<spdlog::logger> _instance;
};

#define LOG_DEBUG(...)    ::Logger::get()->debug(__VA_ARGS__)
#define LOG_INFO(...)     ::Logger::get()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Logger::get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Logger::get()->error(__VA_ARGS__)
#define LOG_FATAL(...)    ::Logger::get()->critical(__VA_ARGS__)

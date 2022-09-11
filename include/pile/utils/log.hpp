#pragma once

#include <spdlog/spdlog.h>

namespace pile::log {
  template <typename T> void setup(const T &pattern) { spdlog::set_pattern(pattern); }
  template <typename T> inline void info(const T &msg) { spdlog::info(msg); }
}  // namespace pile::log

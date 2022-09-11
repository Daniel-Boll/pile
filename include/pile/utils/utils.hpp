#pragma once

#include <pile/utils/common.hpp>

namespace pile::utils {
  bool is_digit(const std::string &str);
  std::vector<std::string> split(const std::string &str, char delimiter);
  std::vector<std::string> split(const std::string &str, const std::string &delimiter);
}  // namespace pile::utils

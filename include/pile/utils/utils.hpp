#pragma once

#include <pile/utils/common.hpp>

namespace pile::utils {
  std::vector<std::string> split(std::string const &str, std::string const &delimiter);
  std::string &rtrim(std::string &s);
  std::string &ltrim(std::string &s);
  std::string &trim(std::string &s);
}  // namespace pile::utils

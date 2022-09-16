#pragma once

#include <pile/utils/common.hpp>

namespace pile::utils {
  bool is_digit(const std::string &str);
  bool is_string(const std::string &str);
  bool is_char(const std::string &str);

  std::vector<std::string> split(const std::string &str, char delimiter);
  std::vector<std::string> split(const std::string &str, const std::string &delimiter);

  std::string split_unless_in_quotes(const std::string &str, char delimiter);
  std::vector<std::string> squish_strings(const std::vector<std::string> &strings);
  std::string unescape_string(const std::string &str);
}  // namespace pile::utils

#include <pile/utils/utils.hpp>

namespace pile::utils {
  bool is_digit(const std::string &str) {
    // Check if the first character is '-', if so, skip remove it
    auto str_copy = str;
    if (str_copy[0] == '-') {
      str_copy.erase(0, 1);
    }

    return !str_copy.empty() && std::find_if(str_copy.begin(), str_copy.end(), [](unsigned char c) {
                                  return !std::isdigit(c);
                                }) == str_copy.end();
  }

  std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
      tokens.push_back(token);
    }
    return tokens;
  }
}  // namespace pile::utils

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

  // starts and ends with "
  bool is_string(const std::string &str) { return str[0] == '"' && str[str.size() - 1] == '"'; }

  std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
      tokens.push_back(token);
    }
    return tokens;
  }

  std::string split_unless_in_quotes(const std::string &str, char delimiter) {
    std::string result;
    bool in_quotes = false;
    for (auto c : str) {
      if (c == '"') {
        in_quotes = !in_quotes;
      }
      if (c == delimiter && !in_quotes) {
        break;
      }
      result += c;
    }
    return result;
  }

  std::vector<std::string> squish_strings(const std::vector<std::string> &strings) {
    std::vector<std::string> result;
    std::string squished_string;
    bool in_quotes = false;
    for (auto &str : strings) {
      if (str[0] == '"' && str[str.size() - 1] == '"') {
        result.push_back(str);
        continue;
      }

      if (str[0] == '"') {
        in_quotes = true;
        squished_string = str;
        continue;
      }

      if (str[str.size() - 1] == '"') {
        in_quotes = false;
        squished_string += " " + str;
        result.push_back(squished_string);
        continue;
      }

      if (in_quotes) {
        squished_string += " " + str;
        continue;
      }

      result.push_back(str);
    }

    return result;
  }

  std::string unescape_string(const std::string &str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
      // TODO: check for other escape sequences
      if (str[i] == '\\' && str[i + 1] == 'n') {
        result += '\n';
        ++i;
      } else if (str[i] == '\\' && str[i + 1] == 't') {
        result += '\t';
        ++i;
      } else {
        result += str[i];
      }
    }

    return result;
  }
}  // namespace pile::utils

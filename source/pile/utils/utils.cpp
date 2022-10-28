#include <pile/utils/utils.hpp>

namespace pile::utils {
  std::vector<std::string> split(std::string const &str, std::string const &delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
      result.push_back(str.substr(start, end - start));
      start = end + delimiter.length();
      end = str.find(delimiter, start);
    }
    result.push_back(str.substr(start, end));
    return result;
  }
}  // namespace pile::utils

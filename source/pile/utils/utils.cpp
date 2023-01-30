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

  std::string &ltrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    return s;
  }

  std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
    return s;
  }

  std::string &trim(std::string &s) { return ltrim(rtrim(s)); }
}  // namespace pile::utils

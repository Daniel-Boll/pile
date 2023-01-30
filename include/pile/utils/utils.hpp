#pragma once

#include <pile/utils/common.hpp>

namespace pile::utils {
  std::vector<std::string> split(std::string const &str, std::string const &delimiter);
  std::string &rtrim(std::string &s);
  std::string &ltrim(std::string &s);
  std::string &trim(std::string &s);

  template <typename ReturnType, typename ClassType, typename... Args> class Memoizer {
  public:
    Memoizer(ReturnType (ClassType::*func)(Args...),
             std::map<std::tuple<Args...>, ReturnType> &memo)
        : func_(func), memo_(memo){};
    ReturnType operator()(ClassType *object, Args... args) {
      auto key = std::make_tuple(args...);
      if (memo_.find(key) == memo_.end()) {
        memo_[key] = (object->*func_)(args...);
      }
      return memo_[key];
    }

  private:
    std::map<std::tuple<Args...>, ReturnType> &memo_;
    ReturnType (ClassType::*func_)(Args...);
  };

  template <typename ReturnType, typename ClassType, typename... Args>
  Memoizer<ReturnType, ClassType, Args...> make_memoizer(
      ReturnType (ClassType::*func)(Args...), std::map<std::tuple<Args...>, ReturnType> &memo) {
    return Memoizer<ReturnType, ClassType, Args...>(func, memo);
  }
}  // namespace pile::utils

#pragma once

#include <pile/utils/common.hpp>

#define PILE_VERSION "0.1.2"
#define uninplemented()                                             \
  spdlog::error("Unimplemented function: {}", __PRETTY_FUNCTION__); \
  exit(1);

#define LOCATION() __FILE__ << ":" << __LINE__

#define assert_msg(x, msg)                                                            \
  if (!(x)) {                                                                         \
    std::cout << "[" LOCATION() << "] " << UNDERLINE YELLOW #x << RESET GREEN " " msg \
              << RESET "\n";                                                          \
  }

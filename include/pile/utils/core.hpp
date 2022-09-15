#pragma once

#include <pile/utils/common.hpp>

#define PILE_VERSION "0.0.8"
#define uninplemented()                                             \
  spdlog::error("Unimplemented function: {}", __PRETTY_FUNCTION__); \
  exit(1);

#define LOCATION() __FILE__ << ":" << __LINE__

#define ESC "\x1b"
#define GREEN CSI "32m"
#define UNDERLINE CSI "4m"
#define YELLOW CSI "33m"
#define RESET CSI "0m"
#define CSI ESC "["

#define assert_msg(x, msg)                                                            \
  if (!(x)) {                                                                         \
    std::cout << "[" LOCATION() << "] " << UNDERLINE YELLOW #x << RESET GREEN " " msg \
              << RESET "\n";                                                          \
  }

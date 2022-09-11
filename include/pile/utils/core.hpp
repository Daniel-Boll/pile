#pragma once

#include <pile/utils/common.hpp>

#define PILE_VERSION "0.0.1"
#define uninplemented()                                             \
  spdlog::error("Unimplemented function: {}", __PRETTY_FUNCTION__); \
  exit(1);


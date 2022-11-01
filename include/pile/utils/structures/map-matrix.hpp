#pragma once

#include <spdlog/fmt/bundled/format.h>

#include <pile/utils/common.hpp>

class MapMatrix {
private:
  std::map<std::pair<std::string, std::string>, std::string> data_;

public:
  MapMatrix() = default;
  MapMatrix(const MapMatrix &) = default;
  MapMatrix(MapMatrix &&) = default;
  MapMatrix &operator=(const MapMatrix &) = default;
  MapMatrix &operator=(MapMatrix &&) = default;

  std::string &operator()(const std::string &i, const std::string &j) { return data_[{i, j}]; }

  const std::string &operator()(const std::string &i, const std::string &j) const {
    return data_.at({i, j});
  }

  // contains
  bool contains(const std::string &i, const std::string &j) const {
    return data_.find({i, j}) != data_.end();
  }

  // Get raw data
  const std::map<std::pair<std::string, std::string>, std::string> &data() const { return data_; }

  void print() const {
    for (const auto &[key, value] : data_) {
      fmt::print("[{}][{}]: {}\n", key.first, key.second, value);
    }
  }

  // Overload <<
  friend std::ostream &operator<<(std::ostream &os, const MapMatrix &m) {
    for (const auto &[key, value] : m.data_)
      os << fmt::format("[{}][{}]: {}\n", key.first, key.second, value);
    os << "\n";

    return os;
  }
};

// Fmt display
template <> struct fmt::formatter<MapMatrix> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext> auto format(const MapMatrix &m, FormatContext &ctx) {
    for (const auto &[key, value] : m.data())
      format_to(ctx.out(), "[{}][{}]: {}\n", key.first, key.second, value);

    return ctx.out();
  }
};

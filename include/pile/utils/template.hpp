#pragma once

#include <algorithm>
#include <cstddef>

template <int N> struct FixedString {
  constexpr FixedString(char const (&s)[N]) { std::copy_n(s, N, this->content); }
  char content[N];
};
template <int N> FixedString(char const (&)[N]) -> FixedString<N>;

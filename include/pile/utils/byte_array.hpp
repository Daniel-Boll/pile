#pragma once

#include <pile/utils/common.hpp>

using byte = uint8_t;

namespace pile {
  class ByteArray {
  private:
    std::vector<byte> data;

  public:
    explicit ByteArray(int32_t size) { data.resize(size); }

    void set(int32_t index, byte value) { data[index] = value; }

    byte get(int32_t index) { return data[index]; }

    int32_t size() { return data.size(); }

    std::vector<byte>& get_data() { return data; }

    // Get interval of bytes through the operator [a, b]
    std::vector<byte> operator()(int32_t a, int32_t b) {
      std::vector<byte> result;
      for (int32_t i = a; i <= b; i++) {
        result.push_back(data[i]);
      }
      return result;
    }
  };
}  // namespace pile

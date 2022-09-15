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

    // Set a sequence of bytes in the memory and return it's address
    int32_t allocate_bytes(int32_t size, const std::vector<byte>& bytes) {
      int32_t address = 0;
      for (int32_t i = 0; i < data.size(); i++) {
        if (data[i] == 0) {
          address = i;
          break;
        }
      }
      for (int32_t i = 0; i < size; i++) {
        data[address + i] = bytes[i];
      }
      return address;
    }

    // Find a sequence of bytes in the memory and return it's address or -1 if not found
    int32_t find_sequence_of_bytes(const std::vector<byte>& bytes) {
      int32_t address = -1;
      for (int32_t i = 0; i < data.size(); i++) {
        if (data[i] == bytes[0]) {
          address = i;
          for (int32_t j = 1; j < bytes.size(); j++) {
            if (data[i + j] != bytes[j]) {
              address = -1;
              break;
            }
          }
          if (address != -1) {
            break;
          }
        }
      }
      return address;
    }
  };
}  // namespace pile
// int32_t index = start;
// while (index + size <= end) {
//   bool found = true;
//   for (int32_t i = 0; i < size; i++) {
//     if (data[index + i] != 0) {
//       found = false;
//       break;
//     }
//   }
//   if (found) {
//     for (int32_t i = 0; i < size; i++) {
//       data[index + i] = bytes[i];
//     }
//     return index;
//   }
//   index++;
// }
// return -1;

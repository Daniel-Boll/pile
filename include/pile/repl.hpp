#pragma once

#include <pile/utils/common.hpp>

#define MEMORY_CAPACITY 320000
#define STRING_CAPACITY 320000

namespace pile {
  class Repl {
  public:
    Repl();
    explicit Repl(int32_t memory_size);
    ~Repl() = default;

    void run();

  private:
    pile::stack<int32_t> stack;
    pile::ByteArray memory;
    bool interpret(const std::vector<OperationData> &operations);
  };
}  // namespace pile

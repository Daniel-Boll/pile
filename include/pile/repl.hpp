#pragma once

#include <pile/utils/common.hpp>

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

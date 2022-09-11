#pragma once

#include <pile/utils/common.hpp>

namespace pile {
  template <typename T> class stack {
  public:
    stack() = default;
    stack(const stack &other) = default;
    stack(stack &&other) noexcept = default;
    stack &operator=(const stack &other) = default;
    stack &operator=(stack &&other) noexcept = default;
    ~stack() = default;

    void push(const T &value) { m_stack.push_back(value); }
    void push(T &&value) { m_stack.push_back(std::move(value)); }

    T pop() {
      T value = std::move(m_stack.back());
      m_stack.pop_back();
      return value;
    }
    T top() const { return m_stack.back(); }

    bool empty() const { return m_stack.empty(); }
    size_t size() const { return m_stack.size(); }

  private:
    std::vector<T> m_stack;
  };

}  // namespace pile

#include <spdlog/spdlog.h>

#include <exception>
#include <pile/parser/grammar.hpp>
#include <string_view>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace pile::Parser::Grammar {
  bool is_production(std::string const &element) {
    // Begin and end with <>
    return element[0] == '<' && element[element.size() - 1] == '>';
  }

  bool is_empty(std::string const &element) { return element == "ε"; }

  void GrammarContent::print() const {
    for (auto const &[production, elements] : content) {
      fmt::print("<{}> -> ", production.content);
      for (auto const &element : elements)
        std::visit(
            overloaded{
                [](Terminal const &terminal) { fmt::print("{} ", terminal.content); },
                [](Production const &production) { fmt::print("<{}> ", production.content); },
                [](Empty const &) { fmt::print("ε "); },
            },
            element);

      fmt::print("\n");
    }
  }
}  // namespace pile::Parser::Grammar

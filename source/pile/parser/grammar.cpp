#include <spdlog/spdlog.h>

#include <exception>
#include <pile/parser/grammar.hpp>
#include <string_view>

namespace pile::Parser::Grammar {
  bool is_production(std::string const &element) {
    // Begin and end with <>
    return element[0] == '<' && element[element.size() - 1] == '>';
  }

  bool is_empty(std::string const &element) { return element == "ε"; }

  bool is_terminal(std::string const &element) {
    return !is_production(element) && !is_empty(element);
  }

  void GrammarContent::print() const {
    for (auto const &[production, elements] : content) {
      fmt::print("<{}> -> ", production.content);
      for (auto const &element : elements)
        std::visit(
            overloaded{
                [](Terminal const &terminal) { fmt::print("{} ", terminal.content); },
                [](Production const &production) { fmt::print("<{}> ", production.content); },
                [](Empty const &empty) { fmt::print("{} ", empty.content); },
                [](Dot const &dot) { fmt::print("{} ", dot.content); },
            },
            element);

      fmt::print("\n");
    }
  }

  Grammar::Symbol find_symbol_next_to_dot(Grammar::Symbols const &symbols) {
    for (auto &symbol : symbols)
      if (std::holds_alternative<Grammar::Dot>(symbol)) {
        auto it = std::find(symbols.begin(), symbols.end(), symbol);
        if (it != symbols.end() && it + 1 != symbols.end()) return *(it + 1);
      }

    return Grammar::Empty{};
  }

  bool is_dot_at_end(Grammar::Symbols const &symbols) {
    return std::ranges::find_if(
               symbols,
               [](Grammar::Symbol symbol) { return std::holds_alternative<Grammar::Dot>(symbol); })
           == symbols.end() - 1;
  }

  std::string to_string(Grammar::Symbol const &symbol) {
    return std::visit(overloaded{
                          [](Grammar::Terminal const &terminal) { return terminal.content; },
                          [](Grammar::Production const &production) {
                            return fmt::format("<{}>", production.content);
                          },
                          [](Grammar::Empty const &empty) { return empty.content; },
                          [](Grammar::Dot const &dot) { return dot.content; },
                      },
                      symbol);
  }
}  // namespace pile::Parser::Grammar

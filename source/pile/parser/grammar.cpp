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
    for (auto &symbol : symbols) {
      if (std::holds_alternative<Grammar::Dot>(symbol)) {
        auto it = std::find(symbols.begin(), symbols.end(), symbol);
        if (it != symbols.end() && it + 1 != symbols.end()) {
          return *(it + 1);
        }
      }
    }

    return Grammar::Empty{};
  }

  Grammar::Symbols remove_dot(Grammar::Symbols const &symbols) {
    Grammar::Symbols new_symbols;
    for (auto &symbol : symbols)
      if (!std::holds_alternative<Grammar::Dot>(symbol)) new_symbols.push_back(symbol);

    return new_symbols;
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

  uint32_t find_production_index(Grammar::GrammarContent const &grammar,
                                 std::pair<Production, Symbols> const &production) {
    auto production_without_dot = remove_dot(production.second);

    auto it = std::ranges::find_if(grammar.content, [&](auto const &p) {
      // The production left and right side should be the same
      if (p.first.content != production.first.content) return false;

      // The production right side should have the same itens
      if (p.second.size() != production_without_dot.size()) return false;

      for (auto i = 0; i < p.second.size(); i++)
        if (to_string(p.second[i]) != to_string(production_without_dot[i])) return false;

      return true;
    });

    if (it == grammar.content.end()) throw std::runtime_error("Production not found.");

    return std::distance(grammar.content.begin(), it);
  }
}  // namespace pile::Parser::Grammar

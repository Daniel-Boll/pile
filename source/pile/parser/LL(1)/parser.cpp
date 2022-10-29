#include <pile/parser/LL(1)/parser.hpp>
#include <pile/utils/template.hpp>
#include <variant>

namespace pile::Parser {
  void LL1::compute_first_set() {
    for (const auto [production, _] : this->grammar.content) {
      this->find_first_set(production);
    }

    // Print first set
    for (const auto [production, first_set] : this->first_set) {
      fmt::print("{} : ", production.content);

      std::vector set_to_vector(first_set.begin(), first_set.end());

      // Reverse the order of the elements
      for (const auto symbol : std::ranges::views::reverse(set_to_vector)) {
        // Get elements from the set
        std::visit(
            overloaded{
                [](const Grammar::Terminal &terminal) { fmt::print("{} ", terminal.content); },
                [](const Grammar::Production &non_terminal) {
                  fmt::print("{} ", non_terminal.content);
                },
                [](const Grammar::Empty &empty) { fmt::print("{} ", empty.content); }},
            symbol);
      }
      std::cout << std::endl;
    }
  }

  void LL1::compute_follow_set() {}

  void LL1::find_first_set(const Grammar::Production &production) {
    // Find the first set for each symbol
    for (auto [_production, _symbols] : this->grammar.content) {
      if (production != _production) continue;

      for (const auto &symbol : _symbols) {
        // If the symbol is a terminal, then the first set is just the symbol
        if (std::holds_alternative<Grammar::Terminal>(symbol)) {
          this->first_set[production].insert(std::get<Grammar::Terminal>(symbol));
          break;
        } else if (std::holds_alternative<Grammar::Empty>(symbol)) {
          this->first_set[production].insert(std::get<Grammar::Empty>(symbol));
          break;
        } else {
          // If the symbol is a non-terminal, then the first set is the first set of the production
          // that produces the symbol
          const auto &non_terminal = std::get<Grammar::Production>(symbol);

          // If the first set of the production that produces the symbol is not computed yet, then
          // compute it
          if (this->first_set.find(non_terminal) == this->first_set.end())
            this->find_first_set(non_terminal);

          // Add the first set of the production that produces the symbol to the first set of the
          // symbol
          std::ranges::copy(
              this->first_set[non_terminal],
              std::inserter(this->first_set[production], this->first_set[production].end()));

          // If the first set of the production that produces the symbol contains the empty symbol,
          // then continue to the next symbol
          if (this->first_set[non_terminal].find(Grammar::Empty{})
              != this->first_set[non_terminal].end())
            continue;

          // If the first set of the production that produces the symbol does not contain the empty
          // symbol, then stop
          break;
        }
      }
    }
  }

  bool LL1::parse(const std::string &input) const { return true; }
}  // namespace pile::Parser

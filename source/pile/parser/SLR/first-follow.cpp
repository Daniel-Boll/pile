#include <pile/parser/SLR/parser.hpp>

namespace pile::Parser {

  SLR *SLR::compute_follow_set() {
    // 1. Add $ to the follow set of the start symbol
    const auto start_symbol = this->grammar.content.begin()->first;

    // fmt::print("Start symbol: {}\n", start_symbol.content);
    this->follow_set[start_symbol].insert(Grammar::Terminal{"$"});

    std::map<Grammar::Production, std::set<Grammar::Symbol>> prev_follow_set;
    while (prev_follow_set != this->follow_set) {
      prev_follow_set = this->follow_set;

      for (const auto [production, _] : this->grammar.content) this->find_follow_set(production);
    }

    // Print follow set
    fmt::print("Follow set:\n");
    for (const auto [production, follow_set] : this->follow_set) {
      fmt::print("{} : ", production.content);

      std::vector set_to_vector(follow_set.begin(), follow_set.end());

      // Reverse the order of the elements
      for (const auto symbol : set_to_vector) {
        // Get elements from the set
        fmt::print("{} ", Grammar::to_string(symbol));
      }
      fmt::print("\n");
    }
    fmt::print("\n");

    return this;
  }

  void SLR::find_first_set(const Grammar::Production &production) {
    // Find the first set for each symbol
    for (auto [_production, _symbols] : this->grammar.content) {
      if (production != _production) continue;

      fmt::print("First set for <{}> : \n", production.content);

      for (const auto &symbol : _symbols) {
        fmt::print("{} \n", Grammar::to_string(symbol));
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

  void SLR::find_follow_set(const Grammar::Production &production) {
    // Find the follow set for each symbol
    for (auto [_production, _symbols] : this->grammar.content) {
      if (production != _production) continue;

      // fmt::print("\nProduction: {}\n", production.content);

      for (const auto &symbol : _symbols) {
        if (!std::holds_alternative<Grammar::Production>(symbol)) continue;

        // If the symbol is a production, then I need to find the next symbol in the symbols
        const auto &non_terminal = std::get<Grammar::Production>(symbol);

        // fmt::print("Non-terminal: {}\n", non_terminal.content);

        // Find the next symbol
        auto next_symbol = std::ranges::find(_symbols, symbol);
        next_symbol++;

        // 1. Case of the last symbol (Get's the follow of the production)
        if (next_symbol == _symbols.end()) {
          // Add the follow set of the production to the follow set of the non-terminal
          std::ranges::copy(
              this->follow_set[production],
              std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()));

          continue;
        }

        // 2. Case of the next symbol being a terminal
        if (std::holds_alternative<Grammar::Terminal>(*next_symbol)) {
          // If the next symbol is a terminal, then add the next symbol to the follow set of the
          // current symbol
          this->follow_set[non_terminal].insert(std::get<Grammar::Terminal>(*next_symbol));
          continue;
        }

        const auto &next_non_terminal = std::get<Grammar::Production>(*next_symbol);

        // If the first set of the next symbol is not computed yet, then compute it
        if (this->first_set.find(next_non_terminal) == this->first_set.end())
          this->find_first_set(next_non_terminal);

        // If the first set of the next symbol contains the empty symbol and the next symbol is the
        // last, then add the follow set of the production to the follow set of the symbol
        if (this->first_set[next_non_terminal].find(Grammar::Empty{})
            != this->first_set[next_non_terminal].end()) {
          std::ranges::copy(
              this->follow_set[production],
              std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()));
        }

        // Add the first set of the next symbol to the follow set of the symbol, except the empty
        // symbol
        std::ranges::copy_if(
            this->first_set[next_non_terminal],
            std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()),
            [](const auto &symbol) { return !std::holds_alternative<Grammar::Empty>(symbol); });
      }
    }
  }
}  // namespace pile::Parser

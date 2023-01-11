#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>

namespace pile::Parser {
  SLR* SLR::populate_state_machine() {
    State* root = state_machine;

    root->expand(this->grammar)->print();

    // Calculate gotos
    {
      for (const auto& [_, symbols] : root->productions) {
        // Get the next symbol
        auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

        if (std::holds_alternative<Grammar::Empty>(nextSymbol)) continue;

        // If the next symbol is not already in the transitions add it's goto
        if (!root->find_symbol_in_state_transitions(nextSymbol)) continue;

        // For every production that this symbols is preceded by the dot
        auto productions = root->find_productions_that_the_symbols_preceeds_the_dot(nextSymbol);
        if (productions.empty()) continue;

        // Generate the goto state
        auto goto_state = generate_goto_state(productions);

        root->push_transition(nextSymbol, &goto_state);
      }
    }

    root->print();

    return this;
  }

  SLR::State SLR::generate_goto_state(
      std::vector<std::pair<Grammar::Production, Grammar::Symbols>> productions) {
    State state;

    for (auto& [production, symbols] : productions) {
      state.push_production(production, symbols);
    }

    state.expand(this->grammar);
    return state;
  }
}  // namespace pile::Parser

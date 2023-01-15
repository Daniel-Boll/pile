#include <pile/parser/SLR/parser.hpp>

#include "pile/parser/grammar.hpp"

namespace pile::Parser {
  using State = SLR::State;

  State* State::push_transition(Grammar::Symbol symbol, std::shared_ptr<State> state) {
    transitions.push_back({symbol, state});
    return this;
  }

  State* State::push_production(Grammar::Production production, Grammar::Symbols symbols) {
    // Check if there's a Dot in the symbols
    auto dot = std::ranges::find_if(symbols, [](Grammar::Symbol symbol) {
      return std::holds_alternative<Grammar::Dot>(symbol);
    });
    if (dot == symbols.end()) {
      symbols.insert(symbols.begin(), Grammar::Dot{});
    } else if (dot + 1 != symbols.end()) {
      std::iter_swap(dot, dot + 1);
    }

    this->productions.push_back({production, symbols});
    return this;
  }

  State* State::next(Grammar::Symbol symbol) {
    for (auto& [s, state] : transitions)
      if (s == symbol) return state.get();

    return nullptr;
  }

  State* State::print_all() {
    if (printed) return this;

    print();
    printed = true;

    for (auto& [symbol, state] : transitions) state->print_all();

    return this;
  }

  State* State::print() {
    using namespace Grammar;

    // Print my address signature
    fmt::print("{} = {{\n", id);
    fmt::print("  .transition = {},\n", Grammar::to_string(transition));
    fmt::print("  .transitions = [");
    for (auto [symbol, state] : transitions) {
      fmt::print("{{{}, {}}}", Grammar::to_string(symbol), state->id);
    }
    fmt::print("],\n");

    fmt::print("  .productions = {{\n");

    for (auto& [production, symbols] : productions) {
      fmt::print("    <{}> -> ", production.content);
      for (auto& symbol : symbols) fmt::print("{} ", Grammar::to_string(symbol));
      fmt::print("\n");
    }

    fmt::print("  }}\n");

    fmt::print("}}\n\n");

    return this;
  }

  bool State::find_production_in_state_productions(const Grammar::Production& production) {
    // Find the production in the state's productions only if the first symbol is a dot
    return std::ranges::find_if(
               this->productions,
               [&production](const auto& pair) {
                 return std::get<0>(pair) == production
                        && std::holds_alternative<Grammar::Dot>(std::get<1>(pair)[0]);
               })
           == this->productions.end();
  }

  bool State::find_symbol_in_state_transitions(const Grammar::Symbol& symbol) {
    return std::ranges::find_if(this->transitions,
                                [&symbol](const auto& pair) { return std::get<0>(pair) == symbol; })
           == this->transitions.end();
  }

  State::StateProductions State::find_productions_that_the_symbols_preceeds_the_dot(
      const Grammar::Symbol& symbol) {
    StateProductions _productions;
    for (auto& [production, symbols] : this->productions) {
      auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

      if (nextSymbol == symbol) _productions.emplace_back(production, symbols);
    }

    return _productions;
  }

  State* State::expand(const Grammar::GrammarContent& grammar) {
    // While the this productions keep changing
    StateProductions last_state;
    do {
      last_state = this->productions;
      // For every state in the state machine
      for (const auto& [_, symbols] : this->productions) {
        // Get the next symbol
        auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

        if (std::holds_alternative<Grammar::Empty>(nextSymbol)) {
          continue;
        }

        // If the next symbol is a production and it's not already in the productions
        if (std::holds_alternative<Grammar::Production>(nextSymbol)
            && this->find_production_in_state_productions(
                std::get<Grammar::Production>(nextSymbol))) {
          auto production = std::get<Grammar::Production>(nextSymbol);

          for (const auto& [_production, _symbols] : grammar.content)
            if (_production == production) this->push_production(_production, _symbols);
        }
      }
    } while (last_state != this->productions);

    return this;
  }

  State* State::calculate_goto(const Grammar::GrammarContent& grammar) {
    for (const auto& [_, symbols] : this->productions) {
      // Get the next symbol
      auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

      if (std::holds_alternative<Grammar::Empty>(nextSymbol)) continue;

      // If the next symbol is not already in the transitions add it's goto
      if (!this->find_symbol_in_state_transitions(nextSymbol)) continue;

      // For every production that this symbols is preceded by the dot
      auto _productions = this->find_productions_that_the_symbols_preceeds_the_dot(nextSymbol);
      if (_productions.empty()) continue;

      // Generate the goto state
      std::shared_ptr<State> goto_state = generate_goto_state(_productions, nextSymbol, grammar);
      this->push_transition(nextSymbol, goto_state);
    }

    return this;
  }

  std::shared_ptr<State> State::generate_goto_state(StateProductions _productions,
                                                    Grammar::Symbol transition_trigger,
                                                    const Grammar::GrammarContent& grammar) {
    if (auto ref = std::ranges::find_if(
            State::memo, [&_productions](const auto& pair) { return pair.first == _productions; });
        ref != memo.end())
      return ref->second;

    auto state = std::make_shared<State>(transition_trigger);
    memo.emplace_back(_productions, state);

    for (auto& [production, symbols] : _productions) state->push_production(production, symbols);

    state->expand(grammar)->calculate_goto(grammar);
    return state;
  }

  std::vector<std::pair<State::StateProductions, std::shared_ptr<State>>> State::memo;
  uint16_t State::counter;
}  // namespace pile::Parser

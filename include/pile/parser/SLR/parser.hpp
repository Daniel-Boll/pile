#pragma once

#include <pile/parser/grammar.hpp>
#include <pile/utils/common.hpp>
#include <pile/utils/template.hpp>
#include <pile/utils/utils.hpp>

namespace pile::Parser {
  class SLR {
  public:
    // A state machine structure that through a given transition (Production or Symbol) can go to
    // another state
    // State = {
    //   0x00 = {
    //     .transitions = [{T, 0x01}, {F, 0x02}],
    //     .productions = {
    //       {S', {"·", T}},
    //       {T, {"·", "F"}},
    //     }
    //   },
    //   0x01 = {
    //     .transitions = [];
    //     .productions = {
    //       {S', {"T", "·"}},
    //       {T, {"T", "·", "*", "F"}},
    //     }
    //   }
    // };
    struct State {
      using StateTransitions = std::vector<std::pair<Grammar::Symbol, std::shared_ptr<State>>>;
      using StateProductions = std::vector<std::pair<Grammar::Production, Grammar::Symbols>>;

      StateTransitions transitions;
      StateProductions productions;
      static std::vector<std::pair<StateProductions, std::shared_ptr<State>>> memo;

      // NOTE: temporary solution, lol.
      bool printed = false;

      State() = default;
      State(const State&) = default;

      State& operator=(const State&) { return *this; }

      State* address() { return this; }

      State* push_transition(Grammar::Symbol symbol, std::shared_ptr<State> state);
      State* push_production(Grammar::Production production, Grammar::Symbols symbols);
      State* next(Grammar::Symbol symbol);

      State* print();
      State* print_all();

      bool find_production_in_state_productions(const Grammar::Production& production);
      bool find_symbol_in_state_transitions(const Grammar::Symbol& symbol);
      StateProductions find_productions_that_the_symbols_preceeds_the_dot(
          const Grammar::Symbol& symbol);

      State* expand(const Grammar::GrammarContent& grammar);
      State* calculate_goto(const Grammar::GrammarContent& grammar);
      std::shared_ptr<State> generate_goto_state(StateProductions _productions,
                                                 const Grammar::GrammarContent& grammar);
    };

  private:
    Grammar::GrammarContent grammar;
    std::unique_ptr<State> state_machine;

  public:
    explicit SLR(Grammar::GrammarContent grammar) : grammar(grammar), state_machine(new State()) {
      auto [production, elements] = this->grammar.content[0];

      state_machine->push_production(production, elements);
    }

    SLR* populate_state_machine();
  };
}  // namespace pile::Parser

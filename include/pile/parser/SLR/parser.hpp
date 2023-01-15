#pragma once

#include <cstdint>
#include <pile/parser/grammar.hpp>
#include <pile/utils/common.hpp>
#include <pile/utils/template.hpp>
#include <pile/utils/utils.hpp>

namespace pile::Parser {
  class SLR {
  public:
    struct State {
      using StateTransition = std::pair<Grammar::Symbol, std::shared_ptr<State>>;
      using StateTransitions = std::vector<StateTransition>;
      using StateProductions = std::vector<std::pair<Grammar::Production, Grammar::Symbols>>;

      static uint16_t counter;
      static std::vector<std::pair<StateProductions, std::shared_ptr<State>>> memo;

      Grammar::Symbol transition;
      StateTransitions transitions;
      StateProductions productions;

      uint16_t id;

      // NOTE: temporary solution, lol.
      bool printed = false;

      explicit State(const Grammar::Symbol& transition) : transition(transition), id(counter++) {}
      State(const State&) = default;

      State& operator=(const State&) = default;

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
                                                 Grammar::Symbol transition_trigger,
                                                 const Grammar::GrammarContent& grammar);
    };

  private:
    Grammar::GrammarContent grammar;
    std::unique_ptr<State> state_machine;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> first_set;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> follow_set;

    SLR* compute_follow_set();

    void find_first_set(const Grammar::Production& production);
    void find_follow_set(const Grammar::Production& production);

    // A custom transition iterator function that takes a predicate to filter the transitions and a
    // callback to execute on each filtered transition.
    template <typename Predicate, typename Callback>
    void transition_iterator(Predicate predicate, Callback callback);

    template <typename Predicate, typename Callback>
    void state_iterator(Predicate predicate, Callback callback);

    std::vector<std::pair<uint16_t, uint16_t>> find_rule_1_matches();
    std::vector<std::pair<uint16_t, Grammar::Symbols>> find_rule_2_matches();

  public:
    explicit SLR(Grammar::GrammarContent grammar)
        : grammar(grammar), state_machine(new State(grammar.content.begin()->first)) {
      auto [production, elements] = this->grammar.content[0];

      state_machine->push_production(production, elements);
    }

    SLR* populate_state_machine();
    SLR* generate_table();
    SLR* parse();
  };
}  // namespace pile::Parser

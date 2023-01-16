#pragma once

#include <cstdint>
#include <pile/parser/grammar.hpp>
#include <pile/utils/common.hpp>
#include <pile/utils/template.hpp>
#include <pile/utils/utils.hpp>

#include "pile/lexer/lexer.hpp"

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

    // Table data structure
    // It should be indexed first by the state id and then by the symbol id.
    // The data that it stores is the action that should be taken, which is either a shift, a
    // reduce, an accept or the id of the production transition.
    struct TableEntry {
      enum class Action { Shift, Reduce, Accept, Goto, Error };

      uint16_t id;
      // The data is a tuple of the symbol, the action and the data identifier.
      std::vector<std::tuple<Grammar::Symbol, Action, uint16_t>> data;

      explicit TableEntry(uint16_t id) : id(id) {}

      static void print_action(Action action, uint16_t data);
    };

    struct Table {
      // The table structure
      std::vector<TableEntry> entries;

      // Operaror overloads
      TableEntry& operator[](uint16_t id);
      TableEntry& operator[](const State& state);

      // Add reduce entries
      Table* add_shift_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data);
      Table* add_reduce_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data);
      Table* add_goto_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data);
      Table* add_accept();
      std::pair<TableEntry::Action, uint16_t> get_action(uint16_t state_id, std::string symbol);
      uint16_t get_goto(uint16_t state_id, std::string symbol);

      Table* print();
      Table* reserve(uint16_t size);
    };

  private:
    Grammar::GrammarContent grammar;
    std::unique_ptr<State> state_machine;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> first_set;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> follow_set;

    Table table;

    SLR* compute_follow_set();

    void find_first_set(const Grammar::Production& production);
    void find_follow_set(const Grammar::Production& production);

    // A custom transition iterator function that takes a predicate to filter the transitions and a
    // callback to execute on each filtered transition.
    template <typename Predicate, typename Callback>
    void transition_iterator(Predicate predicate, Callback callback);

    template <typename Predicate, typename Callback>
    void state_iterator(Predicate predicate, Callback callback);

    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbol>> find_rule_1_matches();
    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbols>> find_rule_2_matches();
    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Production>> find_rule_4_matches();

    State* state_at(uint16_t id);

  public:
    explicit SLR(Grammar::GrammarContent grammar) {
      this->grammar = grammar;
      State::counter = 0;
      this->state_machine = std::make_unique<State>(this->grammar.content.begin()->first);
      auto [production, elements] = this->grammar.content[0];
      state_machine->push_production(production, elements);

      this->compute_follow_set()->populate_state_machine()->generate_table();
    }

    SLR* populate_state_machine();
    SLR* generate_table();
    bool parse(std::vector<Lexer::Token> tokens);
  };
}  // namespace pile::Parser

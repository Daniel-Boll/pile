#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <pile/utils/stack.hpp>
#include <pile/utils/utils.hpp>

#include "spdlog/fmt/bundled/color.h"
#include "spdlog/fmt/ranges.h"

namespace pile::Parser {
  bool SLR::parse(std::vector<Lexer::Token> tokens) {
    // TODO: perhaps add it to the lexer append a $ to the end of the input
    tokens.push_back(Lexer::Token{.file = "test", .type = "$", .lexeme = "$", .position = {1, 1}});

    // The stack of states
    pile::stack<std::string> stack;
    stack.push("0");

    // Action
    std::pair<TableEntry::Action, uint16_t> action;

    // The current token
    auto token = tokens.begin();
    bool computed_successfully = false;

    auto print_token_string_from_iterator = [&tokens](auto it) -> void {
      // highlight as bold the first token and do not print what comes before
      std::string token_string = "";
      for (auto it2 = it; it2 != tokens.end(); it2++) {
        if (it2 == it) {
          token_string
              += fmt::format(fmt::emphasis::bold | fg(fmt::color::yellow), "{}", it2->lexeme);
        } else {
          token_string += fmt::format("{}", it2->lexeme);
        }
      }

      fmt::print("{}\n", token_string);
    };

    for (;;) {
      action = table.get_action(std::stoi(stack.top()), token->type);

      auto [action_type, action_value] = action;

      // stack.print();
      // print_token_string_from_iterator(token);
      // TableEntry::print_action(action_type, action_value);
      // fmt::print("\n");

      if (action_type == TableEntry::Action::Shift) {
        stack.push(token->type);
        stack.push(std::to_string(action_value));
        ++token;
      } else if (action_type == TableEntry::Action::Reduce) {
        auto [production, symbols] = grammar.content.at(action_value);

        for (int i = 0; i < symbols.size() * 2; i++) stack.pop();

        auto s_prime = stack.top();
        auto production_str = fmt::format("<{}>", production.content);

        stack.push(production_str);
        stack.push(std::to_string(table.get_goto(std::stoi(s_prime), production_str)));
      } else if (action_type == TableEntry::Action::Accept) {
        computed_successfully = true;
        break;
      } else {
        break;
      }
    }

    return computed_successfully;
  }

  SLR* SLR::populate_state_machine() {
    // TODO: Check if there's a reasonable way to accomplish this multithreaded.
    state_machine.get()->expand(grammar)->calculate_goto(grammar);
    return this;
  }

  SLR* SLR::generate_table() {
    auto first_rule_pairs = this->find_rule_1_matches();
    auto second_rule_result = this->find_rule_2_matches();
    auto forth_rule_result = this->find_rule_4_matches();

    table.reserve(State::counter);

    for (const auto [id, shift_to, symbol] : first_rule_pairs)
      table.add_shift_entries_to_id(id, {symbol, shift_to});

    // Add reduce entries to id
    for (const auto [id, reduce_to, symbols] : second_rule_result)
      for (const auto& symbol : symbols) table.add_reduce_entries_to_id(id, {symbol, reduce_to});

    // Add goto entries
    for (const auto [id, goto_to, production] : forth_rule_result)
      table.add_goto_entries_to_id(id, {production, goto_to});

    table.add_accept();
    // table.add_accept()->print();

    return this;
  }

  template <typename Predicate, typename Callback>
  void SLR::transition_iterator(Predicate predicate, Callback callback) {
    auto root = state_machine.get();

    std::vector<uint16_t> visited;
    std::deque<State*> queue;

    queue.push_back(root);

    while (!queue.empty()) {
      auto state = queue.front();
      queue.pop_front();

      // If the state has already been visited, skip it.
      if (std::find(visited.begin(), visited.end(), state->id) != visited.end()) continue;

      visited.push_back(state->id);

      for (auto& transition : state->transitions) {
        // If the predicate returns true, execute the callback.
        if (predicate(transition)) callback(state, transition);

        // Push the transition's state to the queue.
        queue.push_back(transition.second.get());
      }
    }
  }

  template <typename Predicate, typename Callback>
  void SLR::state_iterator(Predicate predicate, Callback callback) {
    auto root = state_machine.get();

    std::vector<uint16_t> visited;
    std::deque<State*> queue;

    queue.push_back(root);

    while (!queue.empty()) {
      auto state = queue.front();
      queue.pop_front();

      // If the state has already been visited, skip it.
      if (std::find(visited.begin(), visited.end(), state->id) != visited.end()) continue;

      visited.push_back(state->id);

      // If the predicate returns true, execute the callback.
      if (predicate(state)) callback(state);

      for (auto& transition : state->transitions) {
        // Push the transition's state to the queue.
        queue.push_back(transition.second.get());
      }
    }
  }

  std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbol>> SLR::find_rule_1_matches() {
    auto root = state_machine.get();
    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbol>> matches;

    // Call the transition_iterator with a predicate that checks if the transition is a terminal
    // symbol and a callback that pushes the state id and the transition symbol to the matches
    // vector.
    transition_iterator(
        [](const State::StateTransition& transition) {
          return std::holds_alternative<Grammar::Terminal>(transition.first);
        },
        [&matches](State* state, const State::StateTransition& transition) {
          matches.push_back({state->id, (transition.second)->id, (transition.second)->transition});
        });

    return matches;
  }

  std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbols>> SLR::find_rule_2_matches() {
    auto root = state_machine.get();
    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Symbols>> matches;

    // Call the state_iterator and check if it's a final state.
    state_iterator(
        [](State* state) {
          return Grammar::is_dot_at_end(state->productions[0].second) && state->id > 1;
        },
        [&matches, this](State* state) {
          Grammar::Symbols symbols;
          // Get the symbols of the follow set of the state's production's left side.
          for (const auto& symbol : this->follow_set[state->productions[0].first])
            symbols.push_back(symbol);

          matches.push_back({state->id,
                             Grammar::find_production_index(this->grammar, state->productions[0]),
                             symbols});
        });

    return matches;
  }

  std::vector<std::tuple<uint16_t, uint16_t, Grammar::Production>> SLR::find_rule_4_matches() {
    auto root = state_machine.get();
    std::vector<std::tuple<uint16_t, uint16_t, Grammar::Production>> matches;

    // Call the state_iterator and check if it's a final state.
    transition_iterator(
        [](const State::StateTransition& transition) {
          return std::holds_alternative<Grammar::Production>(transition.first);
        },
        [&matches](State* state, const State::StateTransition& transition) {
          matches.push_back({state->id, (transition.second)->id,
                             std::get<Grammar::Production>(transition.first)});
        });

    return matches;
  }

  SLR::State* SLR::state_at(uint16_t id) {
    SLR::State* state = nullptr;

    // Iterator over the state machine and find the one with the id
    state_iterator(
        [id](State* state) {
          if (state->id == id) return true;
          return false;
        },
        [&state](State* _state) { state = _state; });

    return state;
  }
}  // namespace pile::Parser

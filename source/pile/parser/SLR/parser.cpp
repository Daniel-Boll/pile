#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <pile/utils/utils.hpp>

#include "spdlog/fmt/ranges.h"

namespace pile::Parser {
  SLR* SLR::parse() {
    this->compute_follow_set()->populate_state_machine()->generate_table();

    state_machine.get()->print_all();

    auto first_rule_pairs = this->find_rule_1_matches();
    auto second_rule_result = this->find_rule_2_matches();

    // fmt::print("{}\n", first_rule_pairs);

    fmt::print("================ Rule 2 =================\n");
    for (const auto [id, symbols] : second_rule_result) {
      fmt::print("State: {}\n", id);
      for (const auto symbol : symbols) fmt::print("  {}", Grammar::to_string(symbol));
    }
    fmt::print("\n");

    return this;
  }

  SLR* SLR::populate_state_machine() {
    // TODO: Check if there's a reasonable way to accomplish this multithreaded.
    state_machine.get()->expand(this->grammar)->calculate_goto(this->grammar);
    return this;
  }

  SLR* SLR::generate_table() { return this; }

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

  std::vector<std::pair<uint16_t, uint16_t>> SLR::find_rule_1_matches() {
    auto root = state_machine.get();
    std::vector<std::pair<uint16_t, uint16_t>> matches;

    // Call the transition_iterator with a predicate that checks if the transition is a terminal
    // symbol and a callback that pushes the state id and the transition symbol to the matches
    // vector.
    transition_iterator(
        [](const State::StateTransition& transition) {
          return std::holds_alternative<Grammar::Terminal>(transition.first);
        },
        [&matches](State* state, const State::StateTransition& transition) {
          matches.push_back({state->id, (transition.second)->id});
        });

    return matches;
  }

  std::vector<std::pair<uint16_t, Grammar::Symbols>> SLR::find_rule_2_matches() {
    auto root = state_machine.get();
    std::vector<std::pair<uint16_t, Grammar::Symbols>> matches;

    // Call the state_iterator and check if it's a final state.
    state_iterator(
        [](State* state) {
          auto production = state->productions[0];

          // If the state production have the character ' in it's content, do not consider it a
          // final state.
          if (production.first.content.find('\'') == std::string::npos) return true;

          return Grammar::is_dot_at_end(production.second);
        },
        [&matches, this](State* state) {
          Grammar::Symbols symbols;
          // Get the symbols of the follow set of the state's production's left side.
          for (const auto& symbol : this->follow_set[state->productions[0].first])
            symbols.push_back(symbol);

          // TODO: I'll also have to discover what is the number of the production in the original
          // grammar instead of returning the state id.
          matches.push_back({state->id, symbols});
        });

    return matches;
  }
}  // namespace pile::Parser

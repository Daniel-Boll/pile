#pragma once

#include <pile/parser/grammar.hpp>
#include <pile/utils/common.hpp>

#include "pile/utils/template.hpp"

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
      std::vector<std::pair<Grammar::Symbol, State>> transitions;
      std::vector<std::pair<Grammar::Production, Grammar::Symbols>> productions;

      State() = default;
      State(const State&) = default;

      State& operator=(const State&) { return *this; }

      State* push_transition(Grammar::Symbol symbol, State* state) {
        transitions.push_back({symbol, *state});
        return this;
      }

      State* push_production(Grammar::Production production, Grammar::Symbols symbols) {
        // Check if there's a Dot in the symbols
        if (std::ranges::find_if(
                symbols,
                [](Grammar::Symbol symbol) { return std::holds_alternative<Grammar::Dot>(symbol); })
            == symbols.end()) {
          symbols.insert(symbols.begin(), Grammar::Dot{});
        } else {
          // Move the dot one position to the right
          auto dot = std::ranges::find_if(symbols, [](Grammar::Symbol symbol) {
            return std::holds_alternative<Grammar::Dot>(symbol);
          });

          std::iter_swap(dot, dot + 1);
        }

        productions.push_back({production, symbols});
        return this;
      }

      State* next(Grammar::Symbol symbol) {
        for (auto& [s, state] : transitions)
          if (s == symbol) return &state;

        return nullptr;
      }

      State* print() {
        using namespace Grammar;

        // Print my address signature
        fmt::print("0x{:x} = {{\n", (size_t)this);
        fmt::print("  .transitions = [");
        for (auto [symbol, state] : transitions) {
          fmt::print("{{");
          std::visit(
              overloaded{
                  [](Terminal const& terminal) { fmt::print("{}", terminal.content); },
                  [](Production const& production) { fmt::print("<{}>", production.content); },
                  [](Empty const& empty) { fmt::print("{}", empty.content); },
                  [](Dot const& dot) { fmt::print("{}", dot.content); },
              },
              symbol),
              fmt::print(", 0x{:x}}},", (size_t)&state);
        }
        fmt::print("],\n");

        fmt::print("  .productions = {{\n");

        for (auto& [production, symbols] : productions) {
          fmt::print("    <{}> -> ", production.content);
          for (auto& symbol : symbols)
            std::visit(
                overloaded{
                    [](Terminal const& terminal) { fmt::print("{} ", terminal.content); },
                    [](Production const& production) { fmt::print("<{}> ", production.content); },
                    [](Empty const& empty) { fmt::print("{} ", empty.content); },
                    [](Dot const& dot) { fmt::print("{} ", dot.content); },
                },
                symbol);
          fmt::print("\n");
        }

        fmt::print("  }}\n");

        fmt::print("}}\n");

        // For every possible transition print them
        for (auto& [symbol, state] : transitions) {
          fmt::print("\n");
          state.print();
        }

        return this;
      }

      bool find_production_in_state_productions(const Grammar::Production& production) {
        // Find the production in the state's productions only if the first symbol is a dot
        return std::ranges::find_if(
                   this->productions,
                   [&production](const auto& pair) {
                     return std::get<0>(pair) == production
                            && std::holds_alternative<Grammar::Dot>(std::get<1>(pair)[0]);
                   })
               == this->productions.end();
      }

      bool find_symbol_in_state_transitions(const Grammar::Symbol& symbol) {
        return std::ranges::find_if(
                   this->transitions,
                   [&symbol](const auto& pair) { return std::get<0>(pair) == symbol; })
               == this->transitions.end();
      }

      auto find_productions_that_the_symbols_preceeds_the_dot(const Grammar::Symbol& symbol) {
        std::vector<std::pair<Grammar::Production, Grammar::Symbols>> _productions;
        for (auto& [production, symbols] : this->productions) {
          auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

          if (nextSymbol == symbol) _productions.emplace_back(production, symbols);
        }

        return _productions;
      }

      State* expand(auto const& grammar) {
        // While the this productions keep changing
        std::vector<std::pair<Grammar::Production, Grammar::Symbols>> last_state;
        do {
          last_state = this->productions;
          // For every state in the state machine
          for (const auto& [_, symbols] : this->productions) {
            // Get the next symbol
            auto nextSymbol = Grammar::find_symbol_next_to_dot(symbols);

            if (std::holds_alternative<Grammar::Empty>(nextSymbol)) continue;

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
    };

  private:
    Grammar::GrammarContent grammar;
    State* state_machine;

    State generate_goto_state(
        std::vector<std::pair<Grammar::Production, Grammar::Symbols>> productions);

  public:
    explicit SLR(Grammar::GrammarContent grammar) : grammar(grammar), state_machine(new State()) {
      auto [production, elements] = this->grammar.content[0];

      state_machine->push_production(production, elements);
    }

    SLR* populate_state_machine();
  };

}  // namespace pile::Parser

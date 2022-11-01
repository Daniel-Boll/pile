#pragma once

#include <pile/parser/grammar.hpp>
#include <pile/utils/common.hpp>
#include <pile/utils/structures/map-matrix.hpp>

#include "pile/lexer/lexer.hpp"

// NOTE: It might be better to use a std::unordered_{map,set} instead of a std::{map,set}
namespace pile::Parser {
  class LL1 {
  private:
    Grammar::GrammarContent grammar;

    // First Follow
    std::map<Grammar::Production, std::set<Grammar::Symbol>> first_set;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> follow_set;

    LL1* compute_first_set();
    LL1* compute_follow_set();

    void find_first_set(const Grammar::Production& production);
    void find_follow_set(const Grammar::Production& production);

    // Parsing Table.
    std::vector<std::string> non_terminals;
    std::vector<std::string> terminals;

    MapMatrix parsing_table;

    LL1* compute_parsing_table();
    LL1* fill_lookup_vectors();

    // Helpers
    bool more_than_one_production(const Grammar::Production& production) const;

  public:
    explicit LL1(Grammar::GrammarContent grammar) : grammar(grammar) {
      this->compute_first_set()
          ->compute_follow_set()
          ->fill_lookup_vectors()
          ->compute_parsing_table();
    }

    bool parse(const std::string& input) const;
    bool parse(const std::vector<Lexer::Token>& tokens) const;
  };
}  // namespace pile::Parser

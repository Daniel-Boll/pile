#pragma once

#include <map>
#include <pile/parser/grammar.hpp>
#include <set>

namespace pile::Parser {
  class LL1 {
  private:
    Grammar::GrammarContent grammar;

    std::map<Grammar::Production, std::set<Grammar::Symbol>> first_set;
    std::map<Grammar::Production, std::set<Grammar::Symbol>> follow_set;

    void compute_first_set();
    void compute_follow_set();

    void find_first_set(const Grammar::Production &production);

  public:
    explicit LL1(Grammar::GrammarContent grammar) : grammar(grammar) {
      this->compute_first_set();
      this->compute_follow_set();
    }

    bool parse(const std::string &input) const;
  };
}  // namespace pile::Parser

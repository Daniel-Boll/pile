#pragma once

#include <fstream>
#include <pile/utils/template.hpp>
#include <pile/utils/utils.hpp>
#include <string>
#include <variant>
#include <vector>

namespace pile::Parser::Grammar {
  /* =============== Data Types =============== */
  struct GrammarElement {
    std::string content;

    bool operator==(GrammarElement const &other) const { return content == other.content; }
    bool operator!=(GrammarElement const &other) const { return content != other.content; }

    // Add the possibility to add GrammarElement to a set
    bool operator<(GrammarElement const &other) const { return content < other.content; }

    // Add operator <==>
    auto operator<=>(GrammarElement const &other) const = default;
  };

  struct Terminal : GrammarElement {
    explicit Terminal(std::string const &content) : GrammarElement{content} {}
  };

  struct Empty : GrammarElement {
    explicit Empty() : GrammarElement{"ε"} {}
  };

  struct Dot : GrammarElement {
    explicit Dot() : GrammarElement{"·"} {}
  };

  // NOTE: Perhaps I should change this name to NonTerminal instead
  struct Production : GrammarElement {
    explicit Production(std::string const &content) {
      // Remove <> if present
      if (content[0] == '<' && content[content.size() - 1] == '>') {
        this->content = content.substr(1, content.size() - 2);
      } else {
        this->content = content;
      }
    }
  };

  using Symbol = std::variant<Terminal, Production, Empty, Dot>;
  using Symbols = std::vector<Symbol>;

  using grammar_content_t = std::vector<std::pair<Production, Symbols>>;

  struct GrammarContent {
    grammar_content_t content;

    explicit GrammarContent(grammar_content_t const &content) : content{content} {}

    // Empty grammar
    GrammarContent() = default;

    void print() const;

    static bool equal(grammar_content_t const &lhs, grammar_content_t const &rhs) {
      if (lhs.size() != rhs.size()) return false;

      for (auto i = 0; i < lhs.size(); ++i) {
        if (lhs[i].first != rhs[i].first) return false;

        if (lhs[i].second.size() != rhs[i].second.size()) return false;

        for (auto j = 0; j < lhs[i].second.size(); ++j)
          if (lhs[i].second[j] != rhs[i].second[j]) return false;
      }

      return true;
    }
  };
  /* ========================================== */

  bool is_production(std::string const &element);
  bool is_empty(std::string const &element);
  bool is_terminal(std::string const &element);

  struct ParserProps {
    bool expand;
  };

  // Expects a optional ParserProps, otherwise initialize one with expand to false
  template <FixedString path> GrammarContent parse(ParserProps const &props = {.expand = false}) {
    std::ifstream file(path.content);
    if (!file.is_open()) throw std::string("Could not open file: ") + path.content;

    // Parse GLC file
    GrammarContent grammar;
    std::string line;
    while (std::getline(file, line)) {
      if (line.find("//") != std::string::npos) continue;
      if (line.empty()) continue;

      // Split line into production and productions
      auto const result = pile::utils::split(line, " -> ");
      auto const production = result[0];
      auto const productions = pile::utils::split(result[1], " | ");

      // Check if production is valid
      if (!is_production(production)) throw std::string("Invalid production: ") + production;

      // Parse productions
      for (auto const &_production : productions) {
        // Split production into grammar elements
        auto const grammar_elements = pile::utils::split(_production, " ");

        // Parse grammar elements
        Symbols parsed_grammar_elements;
        std::ranges::transform(grammar_elements, std::back_inserter(parsed_grammar_elements),
                               [](auto const &grammar_element) -> Symbol {
                                 if (is_production(grammar_element))
                                   return Production{grammar_element};
                                 if (is_empty(grammar_element)) return Empty{};
                                 return Terminal{grammar_element};
                               });

        // Add production to grammar
        grammar.content.push_back({Production{production}, parsed_grammar_elements});
      }
    }

    if (props.expand) {
      auto first = grammar.content.front().first.content;
      grammar.content.insert(grammar.content.begin(),
                             {Production{fmt::format("{}'", first)}, {Production{first}}});
    }

    return grammar;
  }

  Grammar::Symbol find_symbol_next_to_dot(Grammar::Symbols const &symbols);
  Grammar::Symbols remove_dot(Grammar::Symbols const &symbols);
  bool is_dot_at_end(Grammar::Symbols const &symbols);

  uint32_t find_production_index(Grammar::GrammarContent const &grammar,
                                 std::pair<Production, Symbols> const &production);

  std::string to_string(Grammar::Symbol const &symbol);
}  // namespace pile::Parser::Grammar


#include <pile/lexer/finite-automatum.hpp>
#include <pile/lexer/macro-expander.hpp>
#include <pile/utils/common.hpp>

namespace pile::Lexer::MacroExpander {
  Lexer::Nameless expand(
      const std::vector<Lexer::Token> &tokens,
      std::unordered_map<std::string, std::vector<Lexer::Token>> *included_macros) {
    std::vector<Lexer::Token> expanded_tokens = tokens;
    std::unordered_map<std::string, std::vector<Lexer::Token>> macros;

    // Macro informations
    std::vector<Lexer::Token> current_macro_body;
    std::string current_macro_name;
    std::vector<std::pair<int32_t, int32_t>> macro_intervals;

    // Create a stack to keep track of the current macro
    pile::stack<int32_t> blocks;
    bool fetching_macro_body = false;
    int32_t index = 0;

    // First loop through; aquire all the macros
    auto token = tokens.begin();
    while (token != tokens.end()) {
      if (token->category == "preprocessor") {
        // Get the macro name
        std::advance(token, 1);
        current_macro_name = token->lexeme;

        // Add the block to the stack
        blocks.push(index);
        std::advance(token, 1);
        fetching_macro_body = true;
        index += 2;
      }
      if (token->lexeme == "if" || token->lexeme == "do") blocks.push(-1);
      if (token->lexeme == "end") {
        if (blocks.empty()) {
          fmt::print("Error: Unexpected end of macro\n");
          return {};
        }

        int32_t block_pos = blocks.pop();
        if (block_pos != -1) {
          macros.insert({current_macro_name, current_macro_body});
          current_macro_body.clear();
          current_macro_name.clear();
          fetching_macro_body = false;
          macro_intervals.push_back({block_pos, index});
        }
      }

      if (fetching_macro_body) current_macro_body.push_back(*token);

      std::advance(token, 1);
      index++;
    }

    // Delete macro intervals
    remove_intervals(expanded_tokens, macro_intervals);

    // Expand macros
    token = expanded_tokens.begin();
    while (token != expanded_tokens.end()) {
      if (token->type == "identifier") {
        auto macro = macros[token->lexeme];
        if (!macro.empty()) {
          token = expanded_tokens.erase(token);
          token = expanded_tokens.insert(token, macro.begin(), macro.end());
        }

        if (included_macros != nullptr) {
          auto included_macro = included_macros->find(token->lexeme);
          if (included_macro != included_macros->end()) {
            token = expanded_tokens.erase(token);
            token = expanded_tokens.insert(token, included_macro->second.begin(),
                                           included_macro->second.end());
          }
        }
      }

      std::advance(token, 1);
    }

    return {.tokens = expanded_tokens, .macros = macros};
  }

  void remove_intervals(std::vector<Lexer::Token> &tokens,
                        std::vector<std::pair<int32_t, int32_t>> intervals) {
    std::vector<bool> flags(tokens.size(), true);
    std::vector<Lexer::Token> ret;
    for (auto interval : intervals)
      std::fill(flags.begin() + interval.first, flags.begin() + interval.second + 1, false);

    for (auto i = 0; i < tokens.size(); i++)
      if (flags[i]) ret.push_back(tokens[i]);

    tokens = ret;
  }
}  // namespace pile::Lexer::MacroExpander

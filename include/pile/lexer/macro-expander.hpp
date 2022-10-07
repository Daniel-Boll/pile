#pragma once

#include <pile/lexer/lexer.hpp>
#include <pile/utils/common.hpp>

namespace pile {
  namespace Lexer {
    namespace MacroExpander {
      Lexer::Nameless expand(
          const std::vector<Lexer::Token> &tokens,
          std::unordered_map<std::string, std::vector<Lexer::Token>> *included_macros = nullptr);

      void remove_intervals(std::vector<Lexer::Token> &tokens,
                            std::vector<std::pair<int32_t, int32_t>> intervals);
    }  // namespace MacroExpander
  }    // namespace Lexer
}  // namespace pile

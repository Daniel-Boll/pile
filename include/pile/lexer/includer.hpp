#pragma once

#include <pile/lexer/finite-automatum.hpp>
#include <pile/lexer/lexer.hpp>
#include <pile/lexer/macro-expander.hpp>

namespace pile {
  namespace Lexer {
    namespace Includer {
      Lexer::Nameless include(const std::string &filename, FiniteAutomatum automatum,
                              std::vector<std::string> *include_paths = nullptr);
      std::string file_location(const std::string &filename,
                                std::vector<std::string> *include_paths = nullptr);
    }  // namespace Includer
  }    // namespace Lexer
}  // namespace pile

#pragma once

#include <pile/utils/common.hpp>

namespace pile {
  namespace Lexer {
    struct Token {
      std::string file;
      std::string type;
      std::string category;
      std::string lexeme;
      std::pair<int, int> position;
    };

    struct Nameless {
      std::vector<Token> tokens;
      std::unordered_map<std::string, std::vector<Token>> macros;
    };

    Lexer::Nameless analyze(const std::string &filename,
                            std::vector<std::string> *include_paths = nullptr);
  }  // namespace Lexer
}  // namespace pile

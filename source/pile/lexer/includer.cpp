#include <pile/lexer/includer.hpp>
#include <pile/lexer/lexer.hpp>

namespace pile::Lexer::Includer {
  Lexer::Nameless include(const std::string &filename, FiniteAutomatum automatum,
                          std::vector<std::string> *include_paths) {
    std::ifstream sample_file(file_location(filename, include_paths));

    std::vector<Lexer::Token> final_tokens;
    std::string line;
    int32_t line_number = 1;
    while (std::getline(sample_file, line)) {
      // Check for include
      if (line.find("include") != std::string::npos) {
        std::string include_file = line.substr(9, line.size() - 10);
        include(include_file, automatum);
        line_number++;
        continue;
      }

      // Compute tokens
      auto tokens = automatum.compute(fmt::format("{}\n", line), filename, line_number);

      // Get all the tokens that are not from the type "space or tab" or "comment"
      std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(final_tokens),
                   [](const Lexer::Token &token) {
                     return token.type != "space or tab" && token.type != "comment";
                   });

      line_number++;
    }

    return MacroExpander::expand(final_tokens);
  }

  std::string file_location(const std::string &filename, std::vector<std::string> *include_paths) {
    const std::vector<std::string> priority_include_paths = {
        "/usr/include",
        "/usr/local/include",
    };

    for (auto &path : priority_include_paths) {
      std::string file = fmt::format("{}/{}", path, filename);
      if (std::filesystem::exists(file)) {
        return file;
      }
    }

    if (include_paths != nullptr) {
      for (auto &path : *include_paths) {
        std::string file = fmt::format("{}/{}", path, filename);
        if (std::filesystem::exists(file)) {
          return file;
        }
      }
    }

    throw std::runtime_error(fmt::format("File {} not found", filename));
  }

}  // namespace pile::Lexer::Includer

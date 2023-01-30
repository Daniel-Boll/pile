#include <spdlog/fmt/fmt.h>

#include <nlohmann/json.hpp>
#include <pile/lexer/includer.hpp>
#include <pile/lexer/lexer.hpp>
#include <pile/lexer/macro-expander.hpp>

using json = nlohmann::json;

// {
//   "keep-comments": true,
//   "include": [
//      "source/pile/lexer/lexer.cpp"
//   ]
// }

namespace pile::Lexer {
  Lexer::Nameless analyze(const std::string &filename, std::vector<std::string> *include_paths) {
    std::ifstream sample_json("./assets/finite-automatum.spec.json");

    if (!sample_json.is_open()) {
      fmt::print("Failed to open sample.json");
      throw "Failed to open sample.json";
    }

    json data = FiniteAutomatum::compute_regex(json::parse(sample_json));

    FiniteAutomatum automatum(data);

    std::unordered_map<std::string, std::vector<Lexer::Token>> macros;

    std::ifstream sample_file(filename);
    std::vector<Lexer::Token> final_tokens;
    std::string line;

    int32_t line_number = 1;
    while (std::getline(sample_file, line)) {
      // Check for include
      if (line.find("include") != std::string::npos) {
        std::string include_file = line.substr(9, line.size() - 10);
        auto [included_tokens, included_macros]
            = Includer::include(include_file, automatum, include_paths);
        std::swap(macros, included_macros);
        macros.insert(included_macros.begin(), included_macros.end());

        // Add the included tokens to the final tokens
        std::ranges::copy(included_tokens, std::back_inserter(final_tokens));

        line_number++;
        continue;
      }

      // Compute tokens
      auto tokens = automatum.compute(fmt::format("{}\n", line), filename, line_number);

      // Get all the tokens that are not from the type "space or tab" or "comment"
      std::ranges::copy_if(tokens, std::back_inserter(final_tokens), [](auto token) {
        // TODO: capture this from flags like: "--keep-comments"
        return token.type != "space-or-tab" && token.type != "comment";
      });

      line_number++;
    }

    // Macro expansion
    auto result = MacroExpander::expand(final_tokens, &macros);
    // for (auto &token : result.tokens) {
    //   std::string print_category = (token.category != "none" && !token.category.empty())
    //                                    ? fmt::format("<{}>", token.category)
    //                                    : "";
    //   fmt::print("({}, {}{}, {}, <{}:{}>)\n", token.file, token.type, print_category, token.lexeme,
    //              token.position.first, token.position.second);
    // }

    return result;
  }
}  // namespace pile::Lexer

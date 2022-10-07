#include <spdlog/fmt/bundled/args.h>

#include <pile/lexer/finite-automatum.hpp>
#include <pile/lexer/lexer.hpp>
#include <pile/utils/common.hpp>

using json = nlohmann::json;

#define NODE_ID_START 0

namespace pile::Lexer {
  FiniteAutomatum::FiniteAutomatum(const json &data) {
    for (auto [idx, node_value] : data["nodes"].items()) {
      Node current_node;
      current_node.id = std::stoi(idx);
      current_node.type = Node::node_type_from_string(node_value["type"]);

      if (current_node.type == NodeType::Normal)
        for (auto &transition : node_value["transitions"].items())
          current_node.transitions.insert({transition.key(), transition.value()});
      else if (current_node.type == NodeType::Computer)
        current_node.computes = node_value["computes"];
      else if (current_node.type == NodeType::Error)
        current_node.error = node_value["error"];

      current_node.exception = node_value["exception"];

      nodes.push_back(current_node);
    }

    for (auto &element : data["keywords"]) keywords.push_back(element);

    for (auto [category, keywords] : data["categories"].items())
      for (auto &keyword : keywords) categories.insert({keyword, category});
  }

  std::vector<Lexer::Token> FiniteAutomatum::compute(const std::string &input,
                                                     const std::string &file, const int32_t &line) {
    std::vector<Lexer::Token> tokens;

    uint_fast8_t current_node_id = NODE_ID_START;
    std::string type = "none";
    std::string current_lexeme = "";
    std::pair<int, int> current_position = {line, 1};

    auto character = input.begin();
    while (character != input.end()) {
      auto current_node = std::find_if(
          nodes.begin(), nodes.end(),
          [current_node_id](const Node &node) { return node.id == current_node_id; });

      switch (current_node->type) {
        case NodeType::Normal: {
          for (auto [regex, next] : current_node->transitions) {
            std::string match = std::string(1, *character);
            if (std::regex_match(match, std::regex(regex))) {
              current_node_id = std::stoi(next);
              current_lexeme += *character;
              break;
            }
          }
          break;
        }
        case NodeType::Computer: {
          std::string token_type = current_node->computes;
          result.category = "none";

          if (current_node->exception == "backtrack") {
            std::advance(character, -1);
            current_lexeme.pop_back();
          } else if (current_node->exception == "identify_token_property") {
            std::advance(character, -1);
            current_lexeme.pop_back();

            auto keyword = std::find(keywords.begin(), keywords.end(), current_lexeme);

            if (keyword != keywords.end()) {
              token_type = "keyword";
              auto category = categories.find(current_lexeme);
              if (category != categories.end()) {
                result.category = category->second;
              }
            }
          } else if (current_node->exception == "remove_cast") {
            // Remove everything after the last . and the .
            current_lexeme = current_lexeme.substr(0, current_lexeme.find_last_of('.'));
          } else if (current_node->exception == "compute_hex") {
            std::advance(character, -1);
            current_lexeme = std::to_string(std::stoi(current_lexeme, nullptr, 16));
          }

          result.type = token_type;
          result.lexeme = current_lexeme;
          result.position = current_position;
          result.file = file;
          tokens.push_back(result);

          current_node_id = NODE_ID_START;
          current_position.second += current_lexeme.size();
          current_lexeme = "";
          std::advance(character, -1);
          break;
        }
        case NodeType::Error: {
          // Red and bold print
          spdlog::error("{}: {}:{} {} got {}\n", "ERROR", current_position.first,
                        current_position.second, current_node->error, current_lexeme);
          // fmt::print("{}: {}:{} {} got {}\n",
          //            fmt::styled("ERROR", fmt::emphasis::bold | fmt::fg(fmt::color::red)),
          //            current_position.first, current_position.second, current_node->error,
          //            current_lexeme);

          current_node_id = NODE_ID_START;
          current_position.second += current_lexeme.size();
          current_lexeme = "";
          std::advance(character, -1);
          break;
        }
      }

      // If I'm the last character
      if (std::next(character) == input.end()) {
        // If the next Node is a computer
        auto next_node = std::find_if(
            nodes.begin(), nodes.end(),
            [current_node_id](const Node &node) { return node.id == current_node_id; });
        if (next_node->type == NodeType::Computer) {
          std::string token_type = next_node->computes;
          if (next_node->exception == "backtrack") {
            current_lexeme.pop_back();
          } else if (next_node->exception == "identify_token_property") {
            current_lexeme.pop_back();

            auto keyword = std::find(keywords.begin(), keywords.end(), current_lexeme);

            if (keyword != keywords.end()) {
              token_type = "keyword";
              auto category = categories.find(current_lexeme);
              if (category != categories.end()) {
                result.category = category->second;
              }
            }
          } else if (next_node->exception == "remove_cast") {
            // Remove everything after the last . and the .
            current_lexeme = current_lexeme.substr(0, current_lexeme.find_last_of('.'));
          } else if (next_node->exception == "compute_hex") {
            current_lexeme.pop_back();
            current_lexeme = std::to_string(std::stoi(current_lexeme, nullptr, 16));
          }

          result.type = token_type;
          result.lexeme = current_lexeme;
          result.position = current_position;
          result.file = file;
          tokens.push_back(result);
        }
      }

      std::advance(character, 1);
    }

    return tokens;
  }

  json FiniteAutomatum::compute_regex(const nlohmann::json &data) {
    json result = data;
    // Read all regex_alias
    fmt::dynamic_format_arg_store<fmt::format_context> pre_store;

    for (auto &regex_alias : data["regex_alias"].items()) {
      std::string key = regex_alias.key();
      std::string value = regex_alias.value();

      pre_store.push_back(fmt::arg(key.c_str(), value));
    }

    fmt::dynamic_format_arg_store<fmt::format_context> expanded_store;
    for (auto &regex_alias : data["regex_alias"].items()) {
      std::string key = regex_alias.key();
      std::string value = regex_alias.value();

      // While there's an available expansion, expand it.
      // TODO: reavaliate a better solution to this.
      while (value.find("{") != std::string::npos) {
        // Expand the first available expansion.
        value = fmt::vformat(value, pre_store);
      }
      expanded_store.push_back(fmt::arg(key.c_str(), value));
    }

    for (auto &node : result["nodes"]) {
      if (node["type"] == "normal") {
        std::map<std::string, std::string> regexes;
        std::vector<std::string> regexes_values;
        bool has_outro = false;
        bool has_outro_star = false;
        for (auto [regex, value] : node["transitions"].items()) {
          if (regex == "{outro}") {
            has_outro = true;
            continue;
          }
          if (regex == "{outro*}") {
            // has_outro_star = true;
            continue;
          }

          auto regex_value = fmt::vformat(regex, expanded_store);
          regexes.insert({regex_value, value});
          regexes_values.push_back(regex_value);
        }

        if (has_outro || has_outro_star) {
          std::ostringstream joined;
          std::copy(regexes_values.rbegin(), regexes_values.rend(),
                    std::ostream_iterator<std::string>(joined, "|"));
          std::string joined_str = joined.str();
          joined_str.pop_back();
          std::string outro_regex;

          if (has_outro) {
            outro_regex = fmt::format("(?:(?!{}).|(\\r|\\n))", joined_str);
          } else {
            outro_regex = fmt::format("(?:(?!{}).)", joined_str);
            result.erase("{outro*}");
          }

          regexes.insert({outro_regex, node["transitions"]["{outro}"]});
        }

        node["transitions"] = regexes;
      }
      if (!node["exception"].is_string()) node["exception"] = "none";
    }

    result.erase("regex_alias");

    return result;
  }

}  // namespace pile::Lexer

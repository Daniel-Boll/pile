#pragma once

#include <nlohmann/json.hpp>
#include <pile/lexer/lexer.hpp>
#include <pile/utils/common.hpp>

namespace pile {
  namespace Lexer {
    class FiniteAutomatum {
    public:
      enum class NodeType {
        Normal,
        Computer,
        Error,
      };

      struct Node {
      public:
        std::string id;
        NodeType type;

        /* ----------------- */
        /* only one per node */
        std::map<std::string, std::string> transitions;
        std::string computes;
        std::string error;
        /* ----------------- */

        std::string exception;

        void print() {
          fmt::print("Node({})\n", id);
          fmt::print("type: {}\n", node_type_to_string());
          if (type == NodeType::Normal) {
            fmt::print("Transitions:\n");
            for (auto &transition : transitions) {
              fmt::print("    transition: {} -> {}\n", transition.first, transition.second);
            }
          } else if (type == NodeType::Computer) {
            fmt::print("computes: {}\n", computes);
          } else if (type == NodeType::Error) {
            fmt::print("error: {}\n", error);
          }
          fmt::print("exception: {}\n\n", exception);
        }

        static NodeType node_type_from_string(const std::string &type) {
          if (type == "normal") {
            return NodeType::Normal;
          } else if (type == "computer") {
            return NodeType::Computer;
          } else if (type == "error") {
            return NodeType::Error;
          }
          return NodeType::Normal;
        }

        std::string node_type_to_string() {
          if (type == NodeType::Normal) {
            return "normal";
          } else if (type == NodeType::Computer) {
            return "compute";
          } else if (type == NodeType::Error) {
            return "error";
          }
          return "none";
        }
      };

    private:
      pile::Lexer::Token result;
      std::vector<Node> nodes;
      std::vector<std::string> keywords;
      std::unordered_map<std::string, std::string> categories;

    public:
      explicit FiniteAutomatum(const nlohmann::json &data);
      ~FiniteAutomatum() = default;

      static nlohmann::json compute_regex(const nlohmann::json &data);

      std::vector<Lexer::Token> compute(const std::string &input, const std::string &filename,
                                        const int32_t &line = 1);
    };
  }  // namespace Lexer
}  // namespace pile

#include <pile/parser.hpp>

namespace pile {
  namespace parser {
    std::unordered_map<std::string, OperationData> op_map({
        {"+", plus()},
        {"-", minus()},
        {"mod", mod()},
        {"dump", dump()},
        {".", dump()},
        {"dup", dup()},
        {"dup2", dup_two()},
        {"drop", drop()},
        {"swap", swap()},
        {"over", over()},
        {"mem", mem()},
        {"<|", store()},
        {"|>", load()},
        {"syscall1", syscall1()},
        {"syscall1!", syscall1_exclamation()},
        {"syscall3", syscall3()},
        {"syscall3!", syscall3_exclamation()},
        {"=", equals()},
        {"if", if_op()},
        {"else", else_op()},
        {"end", end()},
        {">", greater_than()},
        {"<", less_than()},
        {">=", greater_than_or_equal_to()},
        {"<=", less_than_or_equal_to()},
        {"&", bitwise_and()},
        {"|", bitwise_or()},
        {"^", bitwise_xor()},
        {"!", bitwise_not()},
        {"<<", shift_left()},
        {">>", shift_right()},
        {"shift_left", shift_left()},
        {"shift_right", shift_right()},
        {"while", while_op()},
        {"do", do_op()},
        {"macro", macro()},
        {"include", include()},
    });

    TokenData parse_token(const std::string& word) {
      assert_msg(TOKEN_TYPES_COUNT == 4, "Update this function when adding new token types");

      if (utils::is_digit(word)) return TokenData{TokenType::INT, word};
      if (utils::is_char(word)) return TokenData{TokenType::CHARACTER_LITERAL, word};
      if (utils::is_string(word)) {
        return TokenData{TokenType::STRING_LITERAL, word.substr(1, word.size() - 2)};
      }

      return TokenData{TokenType::WORD, word};
    }

    OperationData parse_word_as_op(const TokenData& token) {
      assert_msg(TOKEN_TYPES_COUNT == 4, "Update this function when adding new token types");

      if (token.type == TokenType::INT)
        return push_int(std::stoi(token.value));
      else if (token.type == TokenType::STRING_LITERAL)
        return push_string(token.value);
      else if (token.type == TokenType::CHARACTER_LITERAL) {
        // Remove the first and last character
        auto character = (token.value.substr(1, token.value.size() - 2)).c_str();
        int32_t ascii_code = character[0];

        return (ascii_code == 0) ? push_int(32) : push_int(ascii_code);
      } else if (token.type == TokenType::WORD) {
        auto word = token.value;

        if (op_map.find(word) != op_map.end()) return op_map[word];

        return identifier(word);
      }

      return OperationData{};  // TODO: Perhaps add an unknown operation
    }

    std::vector<OperationData> parse_crossreference_blocks(std::vector<OperationData> operations,
                                                           bool remove_macros) {
      assert_msg(OPERATIONS_COUNT == 38, "Update this function when adding new operations");
      // spdlog::set_level(spdlog::level::debug);

      pile::stack<int32_t> blocks_stack;
      int32_t index = 0;

      std::unordered_map<std::string, Macro> macros;
      std::vector<OperationData> new_operations = operations;
      auto operation = new_operations.begin();

      while (operation != new_operations.end()) {
        operation->instruction_counter = index++;

        spdlog::debug("Parsing operation: {} ({})", operation->get_operation_name(),
                      operation->instruction_counter);
        spdlog::debug("Operations ({})",
                      std::accumulate(std::next(new_operations.begin()), new_operations.end(),
                                      new_operations[0].get_operation_name(),
                                      [](const std::string& a, const OperationData& b) {
                                        return a + ", " + b.get_operation_name();
                                      }));

        switch (operation->operation) {
          case Operation::IF: {
            blocks_stack.push(operation->instruction_counter);
            break;
          }
          case Operation::ELSE: {
            auto if_ic = blocks_stack.pop();
            auto if_op = std::find_if(
                new_operations.begin(), new_operations.end(),
                [if_ic](const OperationData& op) { return op.instruction_counter == if_ic; });

            if (if_op->operation != Operation::IF) {
              spdlog::error("Invalid else block, it must follow an if block and it found {}",
                            if_op->get_operation_name());
              exit(1);
            }

            blocks_stack.push(operation->instruction_counter);

            // Set the closing block of the if block to the else block body
            if_op->closing_block = operation->instruction_counter + 1;
            break;
          }
          case Operation::END: {
            if (blocks_stack.empty()) {
              spdlog::error("Unmatched end");
              exit(1);
            }

            auto block_ic = blocks_stack.pop();
            auto block_op = std::find_if(
                new_operations.begin(), new_operations.end(),
                [block_ic](const OperationData& op) { return op.instruction_counter == block_ic; });

            if (block_op == new_operations.end()) {
              spdlog::error("Invalid block instruction counter");
              exit(1);
            }

            if (block_op->operation == Operation::IF || block_op->operation == Operation::ELSE) {
              // Where to jump (in this case, the next instruction)
              operation->value = operation->instruction_counter + 1;

              block_op->closing_block = operation->instruction_counter;
            } else if (block_op->operation == Operation::DO) {
              // Where to jump (in this case it will return to the next instruction to the while)
              operation->value = block_op->value;
              block_op->closing_block = operation->instruction_counter + 1;
            } else if (block_op->operation == Operation::MACRO) {
              auto macro_begin_it
                  = std::find_if(new_operations.begin(), new_operations.end(),
                                 [block_op](const OperationData& op) {
                                   return op.instruction_counter == block_op->instruction_counter;
                                 });
              auto macro_end_it
                  = std::find_if(new_operations.begin(), new_operations.end(),
                                 [operation](const OperationData& op) {
                                   return op.instruction_counter == operation->instruction_counter;
                                 });

              if (macro_begin_it == new_operations.end()) {
                spdlog::error("Invalid macro begin");
                exit(1);
              }
              if (macro_end_it == new_operations.end()) {
                spdlog::error("Invalid macro end");
                exit(1);
              }

              auto macro_begin = macro_begin_it - new_operations.begin();
              auto macro_end = macro_end_it - new_operations.begin();

              auto condition
                  = new_operations[macro_begin].operation == Operation::MACRO
                    && new_operations[macro_begin + 1].operation == Operation::IDENTIFIER;

              if (!condition) {
                spdlog::error("Invalid macro definition with {} {}",
                              new_operations[macro_begin].get_operation_name(),
                              new_operations[macro_begin + 1].get_operation_name());
                exit(1);
              }

              // Get all the operations inside the macro
              std::vector<OperationData> macro_operations;
              std::copy(new_operations.begin() + macro_begin + 2,
                        new_operations.begin() + macro_end, std::back_inserter(macro_operations));

              macros.insert({block_op->name, Macro{.operations = macro_operations,
                                                   .range = {macro_begin, macro_end}}});

              // for (std::size_t i = 0; i <= (macro_end - (macro_begin - 2)); ++i)
              //   new_operations.erase(operation--);
            } else {
              spdlog::error("Unsupported block type {}", block_op->get_operation_name());
              exit(1);
            }

            break;
          }
          case Operation::WHILE: {
            blocks_stack.push(operation->instruction_counter);
            break;
          }
          case Operation::DO: {
            auto while_ip = blocks_stack.pop();
            auto while_op = std::find_if(
                new_operations.begin(), new_operations.end(),
                [while_ip](const OperationData& op) { return op.instruction_counter == while_ip; });
            if (while_op->operation != Operation::WHILE) {
              spdlog::error("Invalid do block, it must follow a while block");
              exit(1);
            }

            operation->value = while_op->instruction_counter + 1;
            blocks_stack.push(operation->instruction_counter);
            break;
          }
          case Operation::MACRO: {
            // Check if the next operation is a identifier
            auto next_operation = operation + 1;
            if (next_operation->operation != Operation::IDENTIFIER) {
              spdlog::error(
                  "Invalid macro definition, it must be followed by an identifier and found {}\n",
                  next_operation->get_operation_name());
              exit(1);
            }
            blocks_stack.push(operation->instruction_counter);
            operation->name = next_operation->name;

            std::advance(operation, 1);
            break;
          }
          case Operation::IDENTIFIER: {
            if (macros.find(operation->name) != macros.end()) {
              auto macro = macros[operation->name];

              // Print macros
              // spdlog::debug("Found macro: {}", operation->name);
              // for (const auto& op : macro.operations) {
              //   spdlog::debug("{}", op.get_operation_name());
              // }

              // Remove identifier
              operation = new_operations.erase(operation);
              operation = new_operations.insert(operation, macro.operations.begin(),
                                                macro.operations.end());
              continue;
            } else {
              spdlog::error("Unknown identifier: {}", operation->name);
              exit(1);
            }
            break;
          }
          case Operation::INCLUDE: {
            auto next_operation = operation + 1;

            if (next_operation->operation != Operation::PUSH_STRING) {
              spdlog::error("Invalid include, it must be followed by a string and found {}",
                            next_operation->get_operation_name());
              exit(1);
            }

            auto include_path = next_operation->string_content;

            // TODO: perhaps we should let the `extract_operations_from_file` handle this, just
            // catch the exception
            auto include_file = std::ifstream(include_path);
            if (!include_file.is_open()) {
              spdlog::error("Invalid include, file not found: {}", include_path);
              exit(1);
            }

            auto include_operations
                = pile::parser::extract_operations_from_file(include_path, true);

            // Remove the include and the string
            operation = new_operations.erase(operation);
            operation = new_operations.erase(operation);

            operation = new_operations.insert(operation, include_operations.begin(),
                                              include_operations.end());
            continue;
          }
          case Operation::UNKNOWN: {
            spdlog::error("Unknown operation");
            exit(1);
          }
          default: {
            break;
          }
        }

        std::advance(operation, 1);
      }

      // Print all macros
      // for (auto& [name, macro] : macros) {
      //   spdlog::info("Macro: {} @ {} ~> {}", name, std::get<0>(macro.range),
      //                std::get<1>(macro.range));
      //   for (auto& op : macro.operations) {
      //     spdlog::info("  {}", op.get_operation_name());
      //   }
      // }

      std::vector<int32_t> remove_indices = std::accumulate(
          macros.begin(), macros.end(), std::vector<int32_t>{}, [](auto acc, auto macro) {
            auto [begin, end] = macro.second.range;
            for (auto i = begin; i <= end; ++i) {
              acc.push_back(i);
            }
            return acc;
          });

      std::sort(remove_indices.begin(), remove_indices.end());

      // Remove all the macros
      if (remove_macros)
        for (auto it = remove_indices.rbegin(); it != remove_indices.rend(); ++it)
          new_operations.erase(new_operations.begin() + *it);

      spdlog::debug("Operations ({})",
                    std::accumulate(std::next(new_operations.begin()), new_operations.end(),
                                    new_operations[0].get_operation_name(),
                                    [](const std::string& a, const OperationData& b) {
                                      return a + ", " + b.get_operation_name();
                                    }));

      // FIXME: This is not working properly yet.
      if (!blocks_stack.empty()) {
        spdlog::error("Unmatched block{}", blocks_stack.size() > 1 ? "s" : "");
        // Print the unmatched blocks
        while (!blocks_stack.empty()) {
          auto block_ic = blocks_stack.pop();
          auto block_op = new_operations[block_ic];
          spdlog::error("    {}", block_op.get_operation_name());
        }
        exit(1);
      }

      return new_operations;
    }

    std::vector<OperationData> extract_operations_from_file(const std::string& file,
                                                            bool including) {
      std::vector<OperationData> operations;
      std::ifstream fileReader(file);

      if (!fileReader.is_open()) {
        spdlog::error("Failed to open file {}", file);
        exit(1);
      }

      std::string line;
      while (std::getline(fileReader, line)) {
        auto words
            = utils::squish_strings(utils::split(utils::split_unless_in_quotes(line, '\\'), ' '));

        // Parse each word
        for (auto& word : words) {
          if (word.empty()) continue;

          operations.push_back(parse_word_as_op(parse_token(word)));
        }
      }

      return parse_crossreference_blocks(operations, !including);
    }

    std::vector<OperationData> extract_operations_from_multiline(const std::string& lines) {
      std::vector<std::string> lines_vector = pile::utils::split(lines, '\n');
      std::vector<OperationData> operations;

      for (auto& line : lines_vector) {
        auto words
            = utils::squish_strings(utils::split(utils::split_unless_in_quotes(line, '\\'), ' '));

        for (auto& word : words) {
          if (word.empty()) continue;

          operations.push_back(parse_word_as_op(parse_token(word)));
        }
      }

      return parse_crossreference_blocks(operations);
    }

    std::vector<OperationData> extract_operations_from_line(const std::string& line) {
      std::vector<OperationData> operations;
      auto words
          = utils::squish_strings(utils::split(utils::split_unless_in_quotes(line, '\\'), ' '));

      std::transform(words.begin(), words.end(), std::back_inserter(operations),
                     [](auto word) { return parse_word_as_op(parse_token(word)); });

      return parse_crossreference_blocks(operations);
    }
  }  // namespace parser
}  // namespace pile

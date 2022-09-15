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
    });

    // Syscall maps
    std::unordered_map<std::string, OperationData> syscall_map({{"SYS_write", push_int(SYS_write)},
                                                                {"SYS_read", push_int(SYS_read)},
                                                                {"SYS_exit", push_int(SYS_exit)}});

    std::unordered_map<std::string, OperationData> utils_map({{"stdout", push_int(1)},
                                                              {"stderr", push_int(2)}});

    TokenData parse_token(const std::string& word) {
      assert_msg(TOKEN_TYPES_COUNT == 3, "Update this function when adding new token types");

      if (utils::is_digit(word)) return TokenData{TokenType::INT, word};
      if (utils::is_string(word))
        return TokenData{TokenType::STRING_LITERAL, word.substr(1, word.size() - 2)};

      return TokenData{TokenType::WORD, word};
    }

    OperationData parse_word_as_op(const TokenData& token) {
      assert_msg(OPERATIONS_COUNT == 34, "Update this function when adding new operations");
      assert_msg(TOKEN_TYPES_COUNT == 3, "Update this function when adding new token types");

      if (token.type == TokenType::INT) return push_int(std::stoi(token.value));
      if (token.type == TokenType::STRING_LITERAL)
        return push_string(token.value);
      else if (token.type == TokenType::WORD) {
        auto word = token.value;
        if (op_map.find(word) != op_map.end()) return op_map[word];
        if (syscall_map.find(word) != syscall_map.end()) return syscall_map[word];
        if (utils_map.find(word) != utils_map.end()) return utils_map[word];

        return identifier(word);
      }

      return OperationData{};  // TODO: Perhaps add an unknown operation
    }

    std::vector<OperationData> parse_crossreference_blocks(std::vector<OperationData> operations) {
      assert_msg(OPERATIONS_COUNT == 33, "Update this function when adding new operations");
      // spdlog::set_level(spdlog::level::debug);

      pile::stack<int32_t> blocks_stack;
      int32_t index = 0;

      std::unordered_map<std::string, std::vector<OperationData>> macros;
      std::vector<OperationData> new_operations = operations;
      auto operation = new_operations.begin();

      while (operation != new_operations.end()) {
        operation->instruction_counter = index++;

        spdlog::info("Parsing operation: {}", operation->get_operation_name());
        spdlog::info("Operations ({})",
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
            auto if_block = blocks_stack.pop();
            if (new_operations[if_block].operation != Operation::IF) {
              spdlog::error("Invalid else block, it must follow an if block");
              exit(1);
            }

            blocks_stack.push(operation->instruction_counter);

            // Set the closing block of the if block to the else block body
            new_operations[if_block].closing_block = operation->instruction_counter + 1;
            break;
          }
          case Operation::END: {
            if (blocks_stack.empty()) {
              spdlog::error("Unmatched end");
              exit(1);
            }

            auto block_ic = blocks_stack.pop();
            auto block_op = new_operations[block_ic];

            if (block_op.operation == Operation::IF || block_op.operation == Operation::ELSE) {
              // Where to jump (in this case, the next instruction)
              operation->value = operation->instruction_counter + 1;

              block_op.closing_block = operation->instruction_counter;
            } else if (block_op.operation == Operation::DO) {
              // Where to jump (in this case it will return to the next instruction to the while)
              operation->value = block_op.value;
              block_op.closing_block = operation->instruction_counter + 1;
            } else if (block_op.operation == Operation::MACRO) {
              auto macro_begin = block_op.instruction_counter + 2;  // Skip the macro and it's name
              auto macro_end = operation->instruction_counter + 1;

              // Get all the operations inside the macro
              std::vector<OperationData> macro_operations;
              std::copy(new_operations.begin() + macro_begin, new_operations.begin() + macro_end,
                        std::back_inserter(macro_operations));

              macros.insert({block_op.name, macro_operations});

              spdlog::info("Remove {} --(to)-> {} -- {}", macro_begin - 2, macro_end,
                           (macro_end - (macro_begin - 2)));
              for (std::size_t i = 0; i <= (macro_end - (macro_begin - 2)); ++i)
                new_operations.erase(operation--);

              std::advance(operation, 1);
              continue;
            } else {
              spdlog::error("Unsupported block type");
              exit(1);
            }

            new_operations[block_op.instruction_counter] = block_op;
            break;
          }
          case Operation::WHILE: {
            blocks_stack.push(operation->instruction_counter);
            break;
          }
          case Operation::DO: {
            auto while_block = blocks_stack.pop();
            if (new_operations[while_block].operation != Operation::WHILE) {
              spdlog::error("Invalid do block, it must follow a while block");
              exit(1);
            }

            operation->value = while_block + 1;
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
              spdlog::info("Found macro: {}", operation->name);
              auto macro = macros[operation->name];
              // Print the operations in the macro
              for (auto& op : macro) {
                spdlog::info("  {}", op.get_operation_name());
              }

              // Remove identifier
              new_operations.erase(operation);
            } else {
              spdlog::error("Unknown identifier: {}", operation->name);
              exit(1);
            }
            break;
          }
          default: {
            break;
          }
        }

        std::advance(operation, 1);
      }

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

    std::vector<OperationData> extract_operations_from_file(const std::string& file) {
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

      return parse_crossreference_blocks(operations);
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

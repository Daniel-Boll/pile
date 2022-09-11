#include <pile/parser.hpp>

namespace pile {
  namespace parser {
    OperationData parse_word_as_op(const std::string& word) {
      assert_msg(OPERATIONS_COUNT == 24, "Update this function when adding new operations");

      std::unordered_map<std::string, OperationData> op_map = {
          {"+", plus()},
          {"-", minus()},
          // {"*", times()},
          {"mod", mod()},
          {"dump", dump()},
          {".", dump()},
          {"dup", dup()},
          {"dup2", dup_two()},
          {"drop", drop()},
          {"swap", swap()},
          {"mem", mem()},
          {"<|", store()},
          {"|>", load()},
          {"syscall1", syscall1()},
          {"syscall3", syscall3()},
          {"=", equals()},
          {"if", if_op()},
          {"else", else_op()},
          {"end", end()},
          {">", greater_than()},
          {"<", less_than()},
          {">=", greater_than_or_equal_to()},
          {"<=", less_than_or_equal_to()},
          {"while", while_op()},
          {"do", do_op()},
      };

      // Syscall maps
      std::unordered_map<std::string, OperationData> syscall_map = {{"SYS_write", push(SYS_write)},
                                                                    {"SYS_read", push(SYS_read)},
                                                                    {"SYS_exit", push(SYS_exit)}};

      if (pile::utils::is_digit(word)) return push(std::stoi(word));

      // Check if the words is contained in the op_map
      if (op_map.find(word) != op_map.end()) return op_map[word];
      if (syscall_map.find(word) != syscall_map.end()) return syscall_map[word];

      return OperationData{};  // TODO: Perhaps add an unknown operation
    }

    std::vector<OperationData> parse_crossreference_blocks(std::vector<OperationData> operations) {
      assert_msg(OPERATIONS_COUNT == 24, "Update this function when adding new operations");
      // spdlog::set_level(spdlog::level::debug);

      pile::stack<int32_t> blocks_stack;
      int32_t index = 0;

      std::vector<OperationData> new_operations = operations;
      auto operation = new_operations.begin();
      while (operation != new_operations.end()) {
        operation->instruction_counter = index++;

        spdlog::debug("Parsing operation: {}", operation->get_operation_name());

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
          default:
            break;
        }
        std::advance(operation, 1);
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
        // Split line by spaces
        // NOTE: After the addition of the string literals feature, this is will have to be
        // revisited
        std::vector<std::string> words = pile::utils::split(line.substr(0, line.find('\\')), ' ');

        // Parse each word
        for (auto& word : words) {
          if (word.empty()) continue;

          operations.push_back(parse_word_as_op(word));
        }
      }

      return parse_crossreference_blocks(operations);
    }

    std::vector<OperationData> extract_operations_from_line(const std::string& line) {
      std::vector<OperationData> operations;
      // Remove everything after the '\' character (comments) then split the line by spaces
      // NOTE: After the addition of the string literals feature, this is will have to be
      // revisited
      std::vector<std::string> words = pile::utils::split(line.substr(0, line.find('\\')), ' ');

      std::transform(words.begin(), words.end(), std::back_inserter(operations), parse_word_as_op);

      return parse_crossreference_blocks(operations);
    }
  }  // namespace parser
}  // namespace pile

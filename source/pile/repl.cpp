#include <csignal>
#include <iterator>
#include <pile/repl.hpp>

namespace pile {
  Repl::Repl() : memory(1024) {}

  Repl::Repl(int32_t memory_size) : memory(memory_size) {}

  void Repl::run() {
    std::string line;

    bool success = true;
    bool timing = false;

    // Welcome message
    std::cout << "                 ██████  ██ ██      ███████\n";
    std::cout << "                 ██   ██ ██ ██      ██      \n";
    std::cout << "                 ██████  ██ ██      █████   \n";
    std::cout << "                 ██      ██ ██      ██      \n";
    std::cout << "                 ██      ██ ███████ ███████ \n";
    std::cout << "Welcome to pile, a simple stack-based programming language." << std::endl;
    std::cout << "you are using it's oficial compiler/interpreter peak which also implements "
                 "this REPL"
              << std::endl;
    std::cout << "Type 'exit' || 'quit' to exit the REPL." << std::endl;
    std::cout << "Type 'help' to get help." << std::endl;
    std::cout << "The number in the arrow is the size of the stack." << std::endl;
    std::cout << "Try running '1 2 + dump' to get started." << std::endl;
    std::cout << std::endl << std::endl;

    std::signal(SIGINT, [](int) { std::cout << "\033[2J\033[1;1H"; });

    while (true) {
      // Capture Ctrl+L

      std::string successArrow = fmt::format("\033[1;32mpile({})>\033[0m ", stack.size());
      std::string failArrow = fmt::format("\033[1;31mpile({})<\033[0m ", stack.size());
      std::cout << (success ? successArrow : failArrow);
      std::getline(std::cin, line);
      success = true;

      if (line == "exit" || line == "quit") {
        break;
      }

      if (line == "time") {
        timing = !timing;
        std::cout << "Timing is now " << (timing ? "on" : "off") << std::endl;
        continue;
      }

      if (line == "help") {
        std::cout << "pile is a stack-based programming language" << std::endl;
        std::cout << "you are using it's oficial compiler/interpreter peak which also implements "
                     "this REPL"
                  << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  exit: Exit the REPL" << std::endl;
        std::cout << "  help: Print this help message" << std::endl;
        std::cout << "  dump: Dump the top of the stack" << std::endl;
        std::cout << "  <number>: Push a number to the stack" << std::endl;
        std::cout << "  .: Dump the top of the stack" << std::endl;
        std::cout << "  +: Add the top two elements of the stack. Example: 3 2 + (5) (the elements "
                     "are popped)"
                  << std::endl;
        std::cout << "  -: Subtract the top two elements of the stack. Example: 3 2 - (1) (the "
                     "elements are popped)"
                  << std::endl;
        std::cout << "  mod: Modulo the top two elements of the stack. Example: 3 2 mod (1) (the "
                     "elements are popped)"
                  << std::endl;
        std::cout << "  > < >= <= =: Compare the top two elements of the stack. Example: 3 2 > (1) "
                  << std::endl;
        std::cout << "  if: If the top of the stack is true, execute the next command. Example: "
                     "3 2 > 1 if 2 end (2) (the elements are popped)"
                  << std::endl;
        std::cout << "  else: If the top of the if statement was false, execute the next command. "
                     "Example: "
                     "1 2 = 1 if 2 else 3 end (3) (the elements are popped)"
                  << std::endl;
        continue;
      }

      if (line == "clear") {
        std::cout << "\033[2J\033[1;1H";
        continue;
      }

      // Time
      auto start = std::chrono::high_resolution_clock::now();

      // Try to extract operations from the line
      const auto operations = parser::extract_operations_from_line(line);

      // std::for_each(operations.begin(), operations.end(), [=](OperationData op) {
      //   std::cout << "Operation: " << op.get_operation_name() << std::endl
      //             << "Value: " << op.value << std::endl
      //             << "Instruction_counter: " << op.instruction_counter << std::endl
      //             << "Closing_block: " << op.closing_block << std::endl;
      //
      //   // If not the last operation, print a newline
      //   std::cout << std::endl;
      // });

      success = interpret(operations);

      // Time
      auto end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      if (timing) {
        std::cout << "Time: " << duration.count() << " microseconds" << std::endl;
      }
    }
  }

  bool Repl::interpret(const std::vector<OperationData> &operations) {
    static_assert(OPERATIONS_COUNT == 22, "Update this function when adding new operations");
    // spdlog::set_level(spdlog::level::debug);

    auto operation = operations.begin();
    while (operation != operations.end()) {
      spdlog::debug("Interpreting operation: {}", operation->get_operation_name());
      switch (operation->operation) {
        case Operation::PUSH: {
          stack.push(operation->value);
          break;
        }
        case Operation::PLUS: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform addition");
            return false;
          }
          stack.push(stack.pop() + stack.pop());
          break;
        }
        case Operation::MOD: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform modulo");
            return false;
          }
          auto a = stack.pop();
          auto b = stack.pop();
          stack.push(b % a);
          break;
        }
        case Operation::MINUS: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform subtraction");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b - a);

          break;
        }
        case Operation::DUMP: {
          if (stack.empty()) {
            spdlog::error("Cannot dump an empty stack");
            return false;
          }

          std::cout << fmt::format("> {}", stack.pop()) << std::endl;
          break;
        }
        case Operation::DUP: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform duplication");
            return false;
          }

          const auto a = stack.pop();
          stack.push(a);
          stack.push(a);
          break;
        }
        case Operation::DUP2: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform 2 duplication");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b);
          stack.push(a);
          stack.push(b);
          stack.push(a);
          break;
        }
        case Operation::DROP: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform drop");
            return false;
          }

          stack.pop();
          break;
        }
        case Operation::SWAP: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform swap");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(a);
          stack.push(b);
          break;
        }
        case Operation::MEM: {
          stack.push(0);  // There is no such thing as address of .bss section, so in simulation we
                          // just push 0
          break;
        }
        case Operation::EQUAL: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform equality check");
            return false;
          }

          stack.push(stack.pop() == stack.pop());
          break;
        }
        case Operation::IF: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform if");
            return false;
          }

          if (stack.pop() == 0) {
            // Find the operation that matches the end of the if
            const auto end_if_iterator = std::find_if(
                operations.begin(), operations.end(), [operation](const OperationData &op) {
                  return op.instruction_counter == operation->closing_block;
                });

            if (end_if_iterator == operations.end()) {
              spdlog::error("Could not find end of if");
              return false;
            }

            std::advance(operation,
                         end_if_iterator->instruction_counter - operation->instruction_counter);
            continue;
          }
          break;
        }
        case Operation::ELSE: {
          std::advance(operation, operation->closing_block - operation->instruction_counter);
          continue;
        }
        case Operation::END: {
          if (operation->value <= (operations.size() + 1)) {
            // spdlog::info(
            //     "Jumping to operation {}",
            //     operations[operation->value -
            //     operation->instruction_counter].get_operation_name());
            std::advance(operation, operation->value - operation->instruction_counter);
            continue;
          }
        }
        case Operation::GREATER_THAN: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform greater than check");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b > a);
          break;
        }
        case Operation::LESS_THAN: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform less than check");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b < a);
          break;
        }
        case Operation::GREATER_THAN_OR_EQUAL_TO: {
          if (stack.size() < 2) {
            spdlog::error(
                "Not enough values on the stack to perform greater than or equal to check");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b >= a);
          break;
        }
        case Operation::LESS_THAN_EQUAL_OR_EQUAL_TO: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform less than or equal to check");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();
          stack.push(b <= a);
          break;
        }
        case Operation::WHILE: {
          break;
        }
        case Operation::DO: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform do");
            return false;
          }

          if (stack.pop() == 0) {
            if (operation->closing_block <= (operations.size() + 1)) {
              std::advance(operation, operation->closing_block - operation->instruction_counter);
              continue;
            }

            // Find the operation that matches the end of the do
            const auto end_do_iterator = std::find_if(
                operations.begin(), operations.end(), [operation](const OperationData &op) {
                  return op.instruction_counter == operation->closing_block;
                });

            if (end_do_iterator == operations.end()) {
              spdlog::error("Could not find end of do");
              return false;
            }

            std::advance(operation,
                         end_do_iterator->instruction_counter - operation->instruction_counter - 1);
          }

          break;
        }
        case Operation::STORE: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform store");
            return false;
          }

          const auto value = stack.pop();
          const auto address = stack.pop();
          memory.set(address, value);
          break;
        }
        case Operation::LOAD: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform load");
            return false;
          }

          const auto address = stack.pop();
          stack.push(memory.get(address));
          break;
        }
      }

      std::advance(operation, 1);
    }

    return true;
  }
}  // namespace pile

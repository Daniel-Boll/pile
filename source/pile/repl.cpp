#include <unistd.h>

#include <csignal>
#include <iterator>
#include <pile/repl.hpp>

namespace pile {
  Repl::Repl() : memory(MEMORY_CAPACITY + STRING_CAPACITY) {}

  // FIXME: The repl with a custom memory size will also have to declare the capacity of dedicated
  // part of the string
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

      // Check if the line starts with %load
      if (line.find("%load") == 0) {
        std::cout << "pile: " << line << std::endl;
        std::string filename = line.substr(6);
        std::ifstream file(filename);
        if (!file.is_open()) {
          std::cout << "Could not open file " << filename << std::endl;
          continue;
        }

        std::string file_contents((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());

        auto start = std::chrono::high_resolution_clock::now();
        auto operations = parser::extract_operations_from_multiline(file_contents);

        success = interpret(operations);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (timing) std::cout << "Time: " << duration.count() << " microseconds" << std::endl;

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
    assert_msg(OPERATIONS_COUNT == 32, "Update this function when adding new operations");
    // spdlog::set_level(spdlog::level::debug);

    auto operation = operations.begin();
    while (operation != operations.end()) {
      spdlog::debug("Interpreting operation: {}", operation->get_operation_name());
      switch (operation->operation) {
        case Operation::PUSH_STRING: {
          auto result = pile::utils::unescape_string(operation->string);

          // Get the std::vector<uint8_t> from the string
          std::vector<uint8_t> bytes;
          std::copy(result.begin(), result.end(), std::back_inserter(bytes));

          auto address = memory.find_sequence_of_bytes(bytes);
          if (address == -1) {
            address = memory.allocate_bytes(operation->string.length(), bytes);
          }

          stack.push(address);
          stack.push(result.length());

          break;
        }
        case Operation::PUSH_INT: {
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
        case Operation::OVER: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform over");
            return false;
          }

          // b a --- b a b

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(b);
          stack.push(a);
          stack.push(b);
          break;
        }
        case Operation::MEM: {
          stack.push(STRING_CAPACITY);  // There is no such thing as address of .bss section, so in
                                        // simulation we just push 0
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

          spdlog::debug("Inserting value {} at address {}", value, address);

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
        case Operation::SYSCALL1: {
          spdlog::critical("Syscall1 not implemented");
          break;
        }
        case Operation::SYSCALL3: {
          if (stack.size() < 4) {
            spdlog::error("Not enough values on the stack to perform syscall3");
            return false;
          }

          const auto rdx = stack.pop();
          const auto rsi = stack.pop();
          const auto rdi = stack.pop();
          const auto syscall_number = stack.pop();

          if (syscall_number == SYS_write) {
            const auto fd = rdi;
            const auto buffer = rsi;
            const auto count = rdx;

            if (fd != 1 && fd != 2) {
              spdlog::error("Unsupported file descriptor {}", fd);
              return false;
            }

            fputs(reinterpret_cast<const char *>(memory(buffer, buffer + count).data()),
                  fd == 1 ? stdout : stderr);
          } else {
            spdlog::error("Untreated syscall number {}", syscall_number);
          }

          break;
        }
        case Operation::AND: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform and");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(a & b);
          break;
        }
        case Operation::OR: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform or");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(a | b);
          break;
        }
        case Operation::XOR: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform xor");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(a ^ b);
          break;
        }
        case Operation::NOT: {
          if (stack.size() < 1) {
            spdlog::error("Not enough values on the stack to perform not");
            return false;
          }

          const auto a = stack.pop();

          stack.push(~a);
          break;
        }
        case Operation::SHL: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform shl");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(b << a);
          break;
        }
        case Operation::SHR: {
          if (stack.size() < 2) {
            spdlog::error("Not enough values on the stack to perform shr");
            return false;
          }

          const auto a = stack.pop();
          const auto b = stack.pop();

          stack.push(b >> a);
          break;
        }
      }

      std::advance(operation, 1);
    }

    return true;
  }
}  // namespace pile

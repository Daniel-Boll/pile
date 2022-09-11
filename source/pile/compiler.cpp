#include <pile/compiler.hpp>
#include <pile/repl.hpp>

inline static std::string asm_plus() {
  return R"(
    ;; -- plus --
    pop rax
    pop rbx
    add rax, rbx
    push rax
  )";
}

namespace pile {
  void compile(const std::vector<OperationData>& operations, const std::string& output) {
    std::ofstream out(fmt::format("{}.asm", output));
    out << "%define SYS_EXIT 0x3C\n";
    out << "segment .text\n";
    out << dump_declaration;
    out << "global _start\n";
    out << "_start:\n";

    assert_msg(OPERATIONS_COUNT == 24, "Update this function when adding new operations");
    for (auto& op : operations) {
      out << "  .address_" << op.instruction_counter << ":\n";
      switch (op.operation) {
        case Operation::PUSH: {
          out << fmt::format("       ;; -- push {} --\n", op.value);
          out << fmt::format("       push {}\n", op.value);
          break;
        }
        case Operation::PLUS: {
          out << "       ;; -- plus --\n";
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       add rax, rbx\n";
          out << "       push rax\n";
          break;
        }
        case Operation::MINUS: {
          out << "       ;; -- minus --\n";
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       sub rbx, rax\n";
          out << "       push rbx\n";
          break;
        }
        case Operation::DUMP: {
          out << "       ;; -- dump --\n";
          out << "       pop rdi\n";
          out << "       call dump\n";
          break;
        }
        case Operation::DUP: {
          out << "       ;; -- dup --\n";
          out << "       pop rax\n";
          out << "       push rax\n";
          out << "       push rax\n";
          break;
        }
        case Operation::DUP2: {
          out << "       ;; -- dup2 --\n";
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       push rbx\n";
          out << "       push rax\n";
          out << "       push rbx\n";
          out << "       push rax\n";
          break;
        }
        case Operation::DROP: {
          out << "       ;; -- drop --\n";
          out << "       pop rax\n";
          break;
        }
        case Operation::EQUAL: {
          out << "       ;; -- equal --\n";
          out << "       mov rcx, 0x0\n";  // In case of true
          out << "       mov rdx, 0x1\n";  // In case of false
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       cmp rax, rbx\n";
          out << "       cmove rcx, rdx\n";
          out << "       push rcx\n";
          break;
        }
        case Operation::IF: {
          out << "       ;; -- if --\n";
          out << "       pop rax\n";
          out << "       cmp rax, 0x0\n";
          out << "       je .address_" << op.closing_block << "\n";
          break;
        }
        case Operation::ELSE: {
          bool correct_address_interval = (op.value >= 0 && op.value <= operations.size() - 1);
          if (!correct_address_interval) {
            spdlog::error("The `else` instruction must have a valid address to jump to");
            exit(1);
          }
          out << "       ;; -- else --\n";
          out << "       jmp .address_" << op.closing_block << "\n";
          break;
        }
        case Operation::END: {
          // Validate if there is a value
          bool correct_address_interval = (op.value >= 0 && op.value <= operations.size() - 1);
          if (!correct_address_interval) {
            spdlog::error("The `end` instruction must have a valid address to jump to");
            exit(1);
          }
          out << "       ;; -- end --\n";
          out << "       jmp .address_" << op.value << "\n";
          break;
        }
        case Operation::WHILE: {
          out << "       ;; -- while --\n";
          break;
        }
        case Operation::DO: {
          out << "       ;; -- do --\n";
          out << "       pop rax\n";
          out << "       cmp rax, 0x0\n";
          out << "       je .address_" << op.closing_block << "\n";
          break;
        }
        case Operation::GREATER_THAN: {
          out << "       ;; -- greater than --\n";
          out << "       mov rcx, 0x0\n";  // In case of true
          out << "       mov rdx, 0x1\n";  // In case of false
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       cmp rbx, rax\n";
          out << "       cmovg rcx, rdx\n";
          out << "       push rcx\n";
          break;
        }
        case Operation::LESS_THAN: {
          out << "       ;; -- less than --\n";
          out << "       mov rcx, 0x0\n";  // In case of true
          out << "       mov rdx, 0x1\n";  // In case of false
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       cmp rbx, rax\n";
          out << "       cmovl rcx, rdx\n";
          out << "       push rcx\n";
          break;
        }
        case Operation::GREATER_THAN_OR_EQUAL_TO: {
          out << "       ;; -- greater than or equal to --\n";
          out << "       mov rcx, 0x0\n";  // In case of true
          out << "       mov rdx, 0x1\n";  // In case of false
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       cmp rbx, rax\n";
          out << "       cmovge rcx, rdx\n";
          out << "       push rcx\n";
          break;
        }
        case Operation::LESS_THAN_EQUAL_OR_EQUAL_TO: {
          out << "       ;; -- less than or equal to --\n";
          out << "       mov rcx, 0x0\n";  // In case of true
          out << "       mov rdx, 0x1\n";  // In case of false
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       cmp rbx, rax\n";
          out << "       cmovle rcx, rdx\n";
          out << "       push rcx\n";
          break;
        }
        case Operation::MOD: {
          out << "       ;; -- mod --\n";
          out << "       pop rbx\n";
          out << "       pop rax\n";
          out << "       mov rcx, 0x0\n";
          out << "       div rbx\n";
          out << "       push rdx\n";
          break;
        }
        case Operation::SWAP: {
          out << "       ;; -- swap --\n";
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       push rax\n";
          out << "       push rbx\n";
          break;
        }
        case Operation::MEM: {
          out << "       ;; -- mem --\n";
          out << "       push mem\n";
          break;
        }
        case Operation::STORE: {
          out << "       ;; -- store --\n";
          out << "       pop rax\n";
          out << "       pop rbx\n";
          out << "       mov [rbx], al\n";
          break;
        }
        case Operation::LOAD: {
          out << "       ;; -- load --\n";
          out << "       pop rax\n";
          out << "       xor rbx, rbx\n";
          out << "       mov bl, [rax]\n";  // Load only a byte, that why the low part of rbx (bl)
          out << "       push rbx\n";
          break;
        }
        case Operation::SYSCALL1: {
          out << "       ;; -- syscall1 --\n";
          out << "       pop rdi\n";
          out << "       pop rax\n";
          out << "       syscall\n";
          break;
        }
        case Operation::SYSCALL3: {
          out << "       ;; -- syscall3 --\n";
          out << "       pop rdx\n";
          out << "       pop rsi\n";
          out << "       pop rdi\n";
          out << "       pop rax\n";
          out << "       syscall\n";
          break;
        }
      }
    }

    out << "  .address_" << operations.size() << ":\n";
    out << "       ;; -- exit -- \n";
    out << "       mov rax, SYS_EXIT\n";
    out << "       mov rdi, 0\n";
    out << "       syscall\n";
    out << "  segment .bss\n";
    out << "       mem: resq " << MEMORY_CAPACITY << "\n";

    out.close();

    std::string cmd = fmt::format("nasm -f elf64 {} -o {}", output + ".asm", output + ".o");
    if (system(cmd.c_str()) != 0) {
      spdlog::error("Failed to compile assembly file");
      exit(1);
    }

    cmd = fmt::format("ld {} -o {}", output + ".o", output + ".out");
    if (system(cmd.c_str()) != 0) {
      spdlog::error("Failed to link object file");
      exit(1);
    }

    // cmd = fmt::format("rm {}", output + ".o");
    // if (system(cmd.c_str()) != 0) {
    //   spdlog::error("Failed to remove object file");
    //   exit(1);
    // }
  }
}  // namespace pile

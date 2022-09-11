#pragma once

#include <sys/syscall.h>

#include <pile/utils/common.hpp>
#include <pile/utils/core.hpp>

enum class Operation {
  // Stack manipulation
  PUSH,
  DUP,
  DUP2,
  DUMP,
  DROP,
  SWAP,

  // Memory manipulation
  MEM,
  STORE,
  LOAD,

  // Kernel
  SYSCALL1,
  SYSCALL3,

  // Operations
  PLUS,
  MINUS,
  MOD,
  EQUAL,
  GREATER_THAN,
  LESS_THAN,
  GREATER_THAN_OR_EQUAL_TO,
  LESS_THAN_EQUAL_OR_EQUAL_TO,

  // Branching
  IF,
  ELSE,

  // Loop
  WHILE,
  DO,

  // Block control
  END
};
#define OPERATIONS_COUNT 24 // Last syscall1

struct OperationData {
  Operation operation;
  int32_t instruction_counter;

  // NOTE: This perhaps should be a union
  int32_t value;
  int32_t closing_block;

  std::string get_operation_name() const {
    switch (operation) {
      case Operation::PUSH:
        return "PUSH";
      case Operation::DUP:
        return "DUP";
      case Operation::DUP2:
        return "DUP2";
      case Operation::DUMP:
        return "DUMP";
      case Operation::DROP:
        return "DROP";
      case Operation::SWAP:
        return "SWAP";
      case Operation::MEM:
        return "MEM";
      case Operation::STORE:
        return "STORE";
      case Operation::LOAD:
        return "LOAD";
      case Operation::SYSCALL1:
        return "SYSCALL1";
      case Operation::SYSCALL3:
        return "SYSCALL3";
      case Operation::PLUS:
        return "PLUS";
      case Operation::MINUS:
        return "MINUS";
      case Operation::MOD:
        return "MOD";
      case Operation::EQUAL:
        return "EQUAL";
      case Operation::GREATER_THAN:
        return "GREATER_THAN";
      case Operation::LESS_THAN:
        return "LESS_THAN";
      case Operation::GREATER_THAN_OR_EQUAL_TO:
        return "GREATER_THAN_OR_EQUAL_TO";
      case Operation::LESS_THAN_EQUAL_OR_EQUAL_TO:
        return "LESS_THAN_EQUAL_OR_EQUAL_TO";
      case Operation::IF:
        return "IF";
      case Operation::ELSE:
        return "ELSE";
      case Operation::WHILE:
        return "WHILE";
      case Operation::DO:
        return "DO";
      case Operation::END:
        return "END";
      default:
        return "UNKNOWN";
    }
  }
};

namespace pile {
  namespace parser {

    OperationData parse_word_as_op(const std::string& word);
    std::vector<OperationData> parse_crossreference_blocks(std::vector<OperationData> operations);

    std::vector<OperationData> extract_operations_from_file(const std::string& file);
    std::vector<OperationData> extract_operations_from_line(const std::string& line);

    inline OperationData push(int32_t value) {
      return OperationData{.operation = Operation::PUSH, .value = value};
    }
    inline OperationData plus() { return OperationData{.operation = Operation::PLUS}; }
    inline OperationData minus() { return OperationData{.operation = Operation::MINUS}; }
    inline OperationData dump() { return OperationData{.operation = Operation::DUMP}; }
    inline OperationData dup() { return OperationData{.operation = Operation::DUP}; }
    inline OperationData dup_two() { return OperationData{.operation = Operation::DUP2}; }
    inline OperationData drop() { return OperationData{.operation = Operation::DROP}; }
    inline OperationData swap() { return OperationData{.operation = Operation::SWAP}; }
    inline OperationData mem() { return OperationData{.operation = Operation::MEM}; }
    inline OperationData store() { return OperationData{.operation = Operation::STORE}; }
    inline OperationData load() { return OperationData{.operation = Operation::LOAD}; }
    inline OperationData syscall1() { return OperationData{.operation = Operation::SYSCALL1}; }
    inline OperationData syscall3() { return OperationData{.operation = Operation::SYSCALL3}; }
    inline OperationData mod() { return OperationData{.operation = Operation::MOD}; }
    inline OperationData equals() { return OperationData{.operation = Operation::EQUAL}; }
    inline OperationData if_op() { return OperationData{.operation = Operation::IF}; }
    inline OperationData else_op() { return OperationData{.operation = Operation::ELSE}; }
    inline OperationData end() { return OperationData{.operation = Operation::END}; }
    inline OperationData greater_than() {
      return OperationData{.operation = Operation::GREATER_THAN};
    }
    inline OperationData less_than() { return OperationData{.operation = Operation::LESS_THAN}; }
    inline OperationData greater_than_or_equal_to() {
      return OperationData{.operation = Operation::GREATER_THAN_OR_EQUAL_TO};
    }
    inline OperationData less_than_or_equal_to() {
      return OperationData{.operation = Operation::LESS_THAN_EQUAL_OR_EQUAL_TO};
    }
    inline OperationData while_op() { return OperationData{.operation = Operation::WHILE}; }
    inline OperationData do_op() { return OperationData{.operation = Operation::DO}; }

  }  // namespace parser
}  // namespace pile

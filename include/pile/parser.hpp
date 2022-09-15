#pragma once

#include <sys/syscall.h>

#include <pile/utils/common.hpp>
#include <pile/utils/core.hpp>

enum class Operation {
  // Stack manipulation
  PUSH_INT,
  PUSH_STRING,
  DUP,
  DUP2,
  DUMP,
  DROP,
  SWAP,
  OVER,

  // Memory manipulation
  MEM,
  STORE,
  LOAD,

  // Kernel
  SYSCALL1,
  SYSCALL3,
  SYSCALL3_exclamation,

  // Operations
  PLUS,
  MINUS,
  MOD,
  EQUAL,
  GREATER_THAN,
  LESS_THAN,
  GREATER_THAN_OR_EQUAL_TO,
  LESS_THAN_EQUAL_OR_EQUAL_TO,

  // Bitwise operations
  AND,
  OR,
  XOR,
  NOT,
  SHL,
  SHR,

  // Branching
  IF,
  ELSE,

  // Loop
  WHILE,
  DO,

  // Block control
  END,

  // Preprocessor
  MACRO,

  // Identifier
  IDENTIFIER,

  // Unknown
  UNKNOWN,
};
#define OPERATIONS_COUNT 36  // Last operation `UNKNOWN`

enum class TokenType { WORD, INT, STRING_LITERAL };
#define TOKEN_TYPES_COUNT 3  // Last token type `STRING_LITERAL`

struct TokenData {
  TokenType type;
  std::string value;
};

struct OperationData {
  Operation operation;
  int32_t instruction_counter;

  int32_t value;
  std::string string_content;
  std::string name;
  int32_t closing_block;

  ~OperationData() {
    if (operation == Operation::PUSH_STRING) string_content.~basic_string();
  }

  std::string get_operation_name() const {
    switch (operation) {
      case Operation::PUSH_INT:
        return "PUSH_INT";
      case Operation::PUSH_STRING:
        return "PUSH_STRING";
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
      case Operation::OVER:
        return "OVER";
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
      case Operation::SYSCALL3_exclamation:
        return "SYSCALL3_exclamation";
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
      case Operation::AND:
        return "BITWISE_AND";
      case Operation::OR:
        return "BITWISE_OR";
      case Operation::XOR:
        return "BITWISE_XOR";
      case Operation::NOT:
        return "NOT";
      case Operation::SHL:
        return "SHIFT_LEFT";
      case Operation::SHR:
        return "SHIFT_RIGHT";
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
      case Operation::MACRO:
        return "MACRO";
      case Operation::IDENTIFIER:
        return "IDENTIFIER";
      case Operation::UNKNOWN:
        return "UNKNOWN";
      default:
        return "UNKNOWN";
    }
  }
};

struct Macro {
  std::string name;
  std::vector<OperationData> operations;
};

namespace pile {
  namespace parser {
    OperationData parse_word_as_op(const TokenData& token);
    TokenData parse_token(const std::string& word);
    std::vector<OperationData> parse_crossreference_blocks(std::vector<OperationData> operations);

    std::vector<OperationData> extract_operations_from_file(const std::string& file);
    std::vector<OperationData> extract_operations_from_multiline(const std::string& lines);
    std::vector<OperationData> extract_operations_from_line(const std::string& line);

    inline OperationData push_int(int32_t value) {
      return OperationData{.operation = Operation::PUSH_INT, .value = value};
    }
    inline OperationData push_string(const std::string& value) {
      return OperationData{.operation = Operation::PUSH_STRING, .string_content = value};
    }
    inline OperationData plus() { return OperationData{.operation = Operation::PLUS}; }
    inline OperationData minus() { return OperationData{.operation = Operation::MINUS}; }
    inline OperationData dump() { return OperationData{.operation = Operation::DUMP}; }
    inline OperationData dup() { return OperationData{.operation = Operation::DUP}; }
    inline OperationData dup_two() { return OperationData{.operation = Operation::DUP2}; }
    inline OperationData drop() { return OperationData{.operation = Operation::DROP}; }
    inline OperationData swap() { return OperationData{.operation = Operation::SWAP}; }
    inline OperationData over() { return OperationData{.operation = Operation::OVER}; }
    inline OperationData mem() { return OperationData{.operation = Operation::MEM}; }
    inline OperationData store() { return OperationData{.operation = Operation::STORE}; }
    inline OperationData load() { return OperationData{.operation = Operation::LOAD}; }
    inline OperationData syscall1() { return OperationData{.operation = Operation::SYSCALL1}; }
    inline OperationData syscall3() { return OperationData{.operation = Operation::SYSCALL3}; }
    inline OperationData syscall3_exclamation() {
      return OperationData{.operation = Operation::SYSCALL3_exclamation};
    }
    inline OperationData mod() { return OperationData{.operation = Operation::MOD}; }
    inline OperationData equals() { return OperationData{.operation = Operation::EQUAL}; }
    inline OperationData if_op() { return OperationData{.operation = Operation::IF}; }
    inline OperationData else_op() { return OperationData{.operation = Operation::ELSE}; }
    inline OperationData end() { return OperationData{.operation = Operation::END}; }
    inline OperationData macro() { return OperationData{.operation = Operation::MACRO}; }
    inline OperationData identifier(const std::string& name) {
      return OperationData{.operation = Operation::IDENTIFIER, .name = name};
    }
    inline OperationData Unknown() { return OperationData{.operation = Operation::UNKNOWN}; }

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
    inline OperationData bitwise_and() { return OperationData{.operation = Operation::AND}; }
    inline OperationData bitwise_or() { return OperationData{.operation = Operation::OR}; }
    inline OperationData bitwise_xor() { return OperationData{.operation = Operation::XOR}; }
    inline OperationData bitwise_not() { return OperationData{.operation = Operation::NOT}; }
    inline OperationData shift_left() { return OperationData{.operation = Operation::SHL}; }
    inline OperationData shift_right() { return OperationData{.operation = Operation::SHR}; }
    inline OperationData while_op() { return OperationData{.operation = Operation::WHILE}; }
    inline OperationData do_op() { return OperationData{.operation = Operation::DO}; }
  }  // namespace parser
}  // namespace pile

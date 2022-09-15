#pragma once

#include <pile/utils/common.hpp>

namespace pile {
  void compile(const std::vector<OperationData>& operations, const std::string& output);
}

#define MEMORY_CAPACITY 640000
#define STRING_CAPACITY 640000

// TODO: probably declare all those helper functions in a separate file and include them through the
// -i flag in the nasm all this when doing the installation so we can throw the included file in the
// /usr/include/pile folder
const auto dump_declaration
    = "dump:\n"
      "  push rbp\n"
      "  mov rbp, rsp\n"
      "  sub rsp, 96\n"
      "  mov QWORD [rbp-88], rdi\n"
      "  mov QWORD [rbp-8], 1\n"
      "  mov eax, 64\n"
      "  sub rax, QWORD [rbp-8]\n"
      "  mov BYTE [rbp-80+rax], 10\n"
      ".L2:\n"
      "  mov rcx, QWORD [rbp-88]\n"
      "  mov rdx, -3689348814741910323\n"
      "  mov rax, rcx\n"
      "  mul rdx\n"
      "  shr rdx, 3\n"
      "  mov rax, rdx\n"
      "  sal rax, 2\n"
      "  add rax, rdx\n"
      "  add rax, rax\n"
      "  sub rcx, rax\n"
      "  mov rdx, rcx\n"
      "  mov eax, edx\n"
      "  lea edx, [rax+48]\n"
      "  mov eax, 63\n"
      "  sub rax, QWORD [rbp-8]\n"
      "  mov BYTE [rbp-80+rax], dl\n"
      "  add QWORD [rbp-8], 1\n"
      "  mov rax, QWORD [rbp-88]\n"
      "  mov rdx, -3689348814741910323\n"
      "  mul rdx\n"
      "  mov rax, rdx\n"
      "  shr rax, 3\n"
      "  mov QWORD [rbp-88], rax\n"
      "  cmp QWORD [rbp-88], 0\n"
      "  jne .L2\n"
      "  mov eax, 64\n"
      "  sub rax, QWORD [rbp-8]\n"
      "  lea rdx, [rbp-80]\n"
      "  lea rcx, [rdx+rax]\n"
      "  mov rax, QWORD [rbp-8]\n"
      "  mov rdx, rax\n"
      "  mov rsi, rcx\n"
      "  mov edi, 1\n"
      "  mov eax, 0\n"
      "  mov rax, 1\n"
      "  syscall\n"
      "  nop\n"
      "  leave\n"
      "  ret\n";

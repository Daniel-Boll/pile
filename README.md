# <h1 align="center">Pile Lang</h1>

> Pile is a Concatenative Stack-Oriented Programming Language. Pile's name come from the stack concept of _piling_ things.

<p align="center">
  <img src="https://user-images.githubusercontent.com/43689101/190835774-636bfea5-c2c9-49a3-962a-b806c244dff0.png" width="550">
</p>

## Lexer

- [X] BNF
- [X] 3 tipos de datos:
  - Stack: `int`, `float`, `ptr`
  - Memory: `int`, `string`, `ptr`
- [X] Operadores:
  - [X] Aritméticos: `+`, `-`, `*`, `/`, `%`
  - [X] Lógicos/relacional: `&&`, `||`, `!`, `==`, `!=`, `>`, `<`, `>=`, `<=`
  <!-- - [ ] Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>` -->
- [X] Regras de identificadores
- [ ] Palavras reservadas
  - [ ] mod, dump, dup, dup2, drop, swap, over, mem, st8, ld8, st32, ld32, sts, lds, syscall1, syscall1!, syscall3, syscall3!, if, else, while, for, do, macro, end, include, i32, i8, string
- [X] IO
  - [X] `write`
    - [ ] `dump` dump the stack
  - [X] `read`
- [X] Implementação e descrição do autômato

## Implementação

## Installation

## TODO

- [ ] Documentation
- [ ] Refactor components to separatedly do
  - [X] Lexical Analysis
  - [ ] Semantic Analysis
  - [ ] Syntactic Analysis
- [ ] CI/CD test automation
- [ ] Benchmark comparison

## Troubleshooting

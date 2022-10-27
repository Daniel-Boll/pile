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

\begin{bnfgrammar}
<program> ::= <statement>
;;
;;
<statement> ::= <if-statement>
 | <while-statement>
 | <range-statement>
 | <numeric-literal>
 | \textbf{string\_literal}
 | \textbf{id}
 | <assign-to-identifier>
 | <cast>
 | <variable\_declaration>
 | \textbf{arithmetic\_op}
 | <statement> <statement>
;;
;;
<cast> ::= \textbf{\char"003A\char"003A} <type>
;;
<type> ::= \textbf{i32} || \textbf{f32} || \textbf{f64} || \textbf{bool}
;;
<variable\_declaration> ::= <type> \textbf{id}
;;
<assign-to-identifier> ::= \textbf{@} \textbf{id}
;;
<numeric-literal> ::= \textbf{integer\_literal} || \textbf{float\_literal} || \textbf{bool}
;;
;;
<stack-ops> ::= \textbf{swap} || \textbf{dup} || \textbf{drop} || \textbf{over} || \textbf{dup2} || \textbf{dump}
;;
<pop-stack> ::= (<integer-literal> || <stack-ops>)
;;
;;
<if-statement> ::= <if-body> || <if-else-body>
;;
<if-body> ::= \textbf{if} <statement> \textbf{end}
;;
<if-else-body> ::= \textbf{if} <statement> \textbf{else} <statement> \textbf{end}
;;
;;
<while-logic> ::= \textbf{comparison_op} || <numeric-literal> || <stack-ops>
;;
<while-statement> ::= \textbf{while} <while-logic>+ \textbf{do} <statement> \textbf{end}
;;
<range-statement> ::= \textbf{range} <id>? \textbf{do} <statement> \textbf{end}
;;
\end{bnfgrammar}

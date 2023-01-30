//
// #include <numeric>
// #include <pile/parser/LL(1)/parser.hpp>
// #include <pile/utils/template.hpp>
// #include <ranges>
//
// #include "spdlog/fmt/bundled/args.h"
// #include "spdlog/fmt/bundled/color.h"
//
// namespace pile::Parser {
//   LL1 *LL1::compute_first_set() {
//     std::map<Grammar::Production, std::set<Grammar::Symbol>> prev_first_set;
//
//     // While there is no change between the previous and the current first set
//     // compute the first set again
//     do {
//       prev_first_set = this->first_set;
//
//       for (const auto [production, _] : this->grammar.content) this->find_first_set(production);
//     } while (prev_first_set != this->first_set);
//
//     // Print first set
//     fmt::print("First set:\n");
//     for (const auto [production, first_set] : this->first_set) {
//       fmt::print("{} : ", production.content);
//
//       std::vector set_to_vector(first_set.begin(), first_set.end());
//
//       // Reverse the order of the elements
//       for (const auto symbol : set_to_vector) {
//         // Get elements from the set
//         std::visit(
//             overloaded{
//                 [](const Grammar::Terminal &terminal) { fmt::print("{} ", terminal.content); },
//                 [](const Grammar::Production &non_terminal) {
//                   fmt::print("{} ", non_terminal.content);
//                 },
//                 [](const Grammar::Empty &empty) { fmt::print("{} ", empty.content); }},
//             symbol);
//       }
//
//       fmt::print("\n");
//     }
//     fmt::print("\n");
//
//     return this;
//   }
//
//   LL1 *LL1::compute_follow_set() {
//     // 1. Add $ to the follow set of the start symbol
//     const auto start_symbol = this->grammar.content.begin()->first;
//
//     // fmt::print("Start symbol: {}\n", start_symbol.content);
//     this->follow_set[start_symbol].insert(Grammar::Terminal{"$"});
//
//     std::map<Grammar::Production, std::set<Grammar::Symbol>> prev_follow_set;
//     while (prev_follow_set != this->follow_set) {
//       prev_follow_set = this->follow_set;
//
//       for (const auto [production, _] : this->grammar.content) this->find_follow_set(production);
//     }
//
//     // Print follow set
//     fmt::print("Follow set:\n");
//     for (const auto [production, follow_set] : this->follow_set) {
//       fmt::print("{} : ", production.content);
//
//       std::vector set_to_vector(follow_set.begin(), follow_set.end());
//
//       // Reverse the order of the elements
//       for (const auto symbol : set_to_vector) {
//         // Get elements from the set
//         std::visit(
//             overloaded{
//                 [](const Grammar::Terminal &terminal) { fmt::print("{} ", terminal.content); },
//                 [](const Grammar::Production &non_terminal) {
//                   fmt::print("{} ", non_terminal.content);
//                 },
//                 [](const Grammar::Empty &empty) { fmt::print("{} ", empty.content); }},
//             symbol);
//       }
//       fmt::print("\n");
//     }
//     fmt::print("\n");
//
//     return this;
//   }
//
//   void LL1::find_first_set(const Grammar::Production &production) {
//     // Find the first set for each symbol
//     for (auto [_production, _symbols] : this->grammar.content) {
//       if (production != _production) continue;
//
//       for (const auto &symbol : _symbols) {
//         // If the symbol is a terminal, then the first set is just the symbol
//         if (std::holds_alternative<Grammar::Terminal>(symbol)) {
//           this->first_set[production].insert(std::get<Grammar::Terminal>(symbol));
//           break;
//         } else if (std::holds_alternative<Grammar::Empty>(symbol)) {
//           this->first_set[production].insert(std::get<Grammar::Empty>(symbol));
//           break;
//         } else {
//           // If the symbol is a non-terminal, then the first set is the first set of the production
//           // that produces the symbol
//           const auto &non_terminal = std::get<Grammar::Production>(symbol);
//
//           // If the first set of the production that produces the symbol is not computed yet, then
//           // compute it
//           if (this->first_set.find(non_terminal) == this->first_set.end())
//             this->find_first_set(non_terminal);
//
//           // Add the first set of the production that produces the symbol to the first set of the
//           // symbol
//           std::ranges::copy(
//               this->first_set[non_terminal],
//               std::inserter(this->first_set[production], this->first_set[production].end()));
//
//           // If the first set of the production that produces the symbol contains the empty symbol,
//           // then continue to the next symbol
//           if (this->first_set[non_terminal].find(Grammar::Empty{})
//               != this->first_set[non_terminal].end())
//             continue;
//
//           // If the first set of the production that produces the symbol does not contain the empty
//           // symbol, then stop
//           break;
//         }
//       }
//     }
//   }
//
//   void LL1::find_follow_set(const Grammar::Production &production) {
//     // Find the follow set for each symbol
//     for (auto [_production, _symbols] : this->grammar.content) {
//       if (production != _production) continue;
//
//       // fmt::print("\nProduction: {}\n", production.content);
//
//       for (const auto &symbol : _symbols) {
//         if (!std::holds_alternative<Grammar::Production>(symbol)) continue;
//
//         // If the symbol is a production, then I need to find the next symbol in the symbols
//         const auto &non_terminal = std::get<Grammar::Production>(symbol);
//
//         // fmt::print("Non-terminal: {}\n", non_terminal.content);
//
//         // Find the next symbol
//         auto next_symbol = std::ranges::find(_symbols, symbol);
//         next_symbol++;
//
//         // 1. Case of the last symbol (Get's the follow of the production)
//         if (next_symbol == _symbols.end()) {
//           // Add the follow set of the production to the follow set of the non-terminal
//           std::ranges::copy(
//               this->follow_set[production],
//               std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()));
//
//           continue;
//         }
//
//         // 2. Case of the next symbol being a terminal
//         if (std::holds_alternative<Grammar::Terminal>(*next_symbol)) {
//           // If the next symbol is a terminal, then add the next symbol to the follow set of the
//           // current symbol
//           this->follow_set[non_terminal].insert(std::get<Grammar::Terminal>(*next_symbol));
//           continue;
//         }
//
//         const auto &next_non_terminal = std::get<Grammar::Production>(*next_symbol);
//
//         // If the first set of the next symbol is not computed yet, then compute it
//         if (this->first_set.find(next_non_terminal) == this->first_set.end())
//           this->find_first_set(next_non_terminal);
//
//         // If the first set of the next symbol contains the empty symbol and the next symbol is the
//         // last, then add the follow set of the production to the follow set of the symbol
//         if (this->first_set[next_non_terminal].find(Grammar::Empty{})
//             != this->first_set[next_non_terminal].end()) {
//           std::ranges::copy(
//               this->follow_set[production],
//               std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()));
//         }
//
//         // Add the first set of the next symbol to the follow set of the symbol, except the empty
//         // symbol
//         std::ranges::copy_if(
//             this->first_set[next_non_terminal],
//             std::inserter(this->follow_set[non_terminal], this->follow_set[non_terminal].end()),
//             [](const auto &symbol) { return !std::holds_alternative<Grammar::Empty>(symbol); });
//       }
//     }
//   }
//
//   LL1 *LL1::fill_lookup_vectors() {
//     // Fill the non-terminals and terminals.
//     for (const auto &[production, _] : this->grammar.content)
//       this->non_terminals.push_back(production.content);
//
//     for (const auto &[_, symbols] : this->grammar.content)
//       for (const auto &symbol : symbols)
//         if (std::holds_alternative<Grammar::Terminal>(symbol))
//           this->terminals.push_back(std::get<Grammar::Terminal>(symbol).content);
//
//     return this;
//   }
//
//   bool LL1::more_than_one_production(const Grammar::Production &production) const {
//     return std::ranges::count_if(this->grammar.content,
//                                  [&production](const auto &grammar_value) {
//                                    return production == grammar_value.first;
//                                  })
//            > 1;
//   }
//
//   LL1 *LL1::compute_parsing_table() {
//     // // For each production in the grammar (foreach prod A -> alpha in G)
//     // for (const auto &[production, symbols] : this->grammar.content) {
//     //   fmt::print("\nProduction: {}\n", production.content);
//     //
//     //   // For each symbol in the first set of the production (foreach symbol a in FIRST(A))
//     //   for (const auto &first_set_symbols : this->first_set[production]) {
//     //     std::string symbol
//     //         = std::visit([](const auto &symbol) { return symbol.content; }, first_set_symbols);
//     //     std::string symbols_string = std::accumulate(
//     //         symbols.begin(), symbols.end(), std::string{}, [](const auto &acc, const auto
//     //         &symbol) {
//     //           return acc + std::visit([](const auto &symbol) { return symbol.content; }, symbol);
//     //         });
//     //
//     //     parsing_table(production.content, symbol) = symbols_string;
//     //   }
//     // }
//
//     // For each production in the grammar (foreach prod A -> alpha in G)
//     for (const auto &[production, symbols] : this->grammar.content) {
//       fmt::print("\nProduction: {}\n", production.content);
//
//       // For each symbol in the rhs of the production (foreach symbol a in alpha)
//       bool finished = false;
//       // next_list
//       std::set<std::string> next_list;
//       for (const auto &symbol : symbols) {
//         // If the symbols is a terminal add it to the next_list
//         if (std::holds_alternative<Grammar::Terminal>(symbol)) {
//           next_list.insert(std::get<Grammar::Terminal>(symbol).content);
//           finished = true;
//           continue;
//         }
//
//         // Add the first set of the symbol to the next_list transforming it to a string
//         // except the $ and the empty symbol
//         std::vector<std::string> free_symbols;
//         for (const auto &first_set_symbols : this->first_set[std::get<Grammar::Production>(symbol)])
//           if (std::holds_alternative<Grammar::Terminal>(first_set_symbols))
//             free_symbols.push_back(std::get<Grammar::Terminal>(first_set_symbols).content);
//
//         std::ranges::copy(free_symbols, std::inserter(next_list, next_list.end()));
//       }
//
//       // If the next_list contains the empty symbol, then add the follow set of the production to
//       // the next_list
//       if (!finished)
//         std::ranges::transform(this->follow_set[production],
//                                std::inserter(next_list, next_list.end()), [](const auto &symbol) {
//                                  return std::visit(
//                                      [](const auto &symbol) { return symbol.content; }, symbol);
//                                });
//
//       // For each symbol in the next_list (foreach symbol a in next_list)
//       for (const auto &symbol : next_list) {
//         std::string symbols_string = std::accumulate(
//             symbols.begin(), symbols.end(), std::string{}, [](const auto &acc, const auto &symbol) {
//               return acc + std::visit([](const auto &symbol) { return symbol.content; }, symbol);
//             });
//
//         parsing_table(production.content, symbol) = symbols_string;
//       }
//     }
//
//     return this;
//   }
//
//   bool LL1::parse(const std::string &input) const {
//     fmt::print("Parsing table:\n{}\n", this->parsing_table);
//
//     std::vector<std::string> stack;
//     std::string input_tape = input + " $";
//     const auto first_production = fmt::format("<{}>", this->grammar.content[0].first.content);
//
//     stack.push_back("$");
//     stack.push_back(first_production);
//
//     do {
//       std::string top = stack.back();
//       // Get next word from input tape
//       std::string input_symbol = input_tape.substr(0, input_tape.find(" "));
//
//       // Copy the stack content minus the top
//       std::string stack_content;
//       std::ranges::for_each(
//           stack, [&stack_content](const auto &symbol) { stack_content += symbol + " "; });
//       stack_content = stack_content.substr(0, stack_content.find_last_of(" "));
//       stack_content = stack_content.substr(0, stack_content.find_last_of(" "));
//       stack_content.erase(std::remove(stack_content.begin(), stack_content.end(), ' '),
//                           stack_content.end());
//
//       // Copy the input tape content minus the input symbol
//       std::string input_tape_content = input_tape.substr(input_tape.find(" ") + 1);
//       // if input tape content is just $, then set it to empty
//       if (input_tape_content == "$") input_tape_content = "";
//
//       std::string print_top = fmt::format(
//           fmt::emphasis::underline | fmt::emphasis::bold | fg(fmt::color::gold), "{}", top);
//       std::string print_input_symbol
//           = fmt::format(fmt::emphasis::underline | fmt::emphasis::bold | fg(fmt::color::gold), "{}",
//                         input_symbol);
//
//       fmt::print("{}{}\t | {} {}\n", stack_content, print_top, print_input_symbol,
//                  input_tape_content);
//
//       // If top is a terminal
//       if (Grammar::is_terminal(top)) {
//         // If top is the same as the first character of the input tape
//         if (top != input_symbol) return false;
//
//         // Pop top from the stack
//         stack.pop_back();
//         // Remove the input symbol from the input tape
//         input_tape = input_tape.substr(input_tape.find(" ") + 1);
//       } else {
//         top = top.substr(1, top.size() - 2);
//         // If the parsing table has that production and terminal
//         if (!this->parsing_table.contains(top, input_symbol)) {
//           spdlog::error("ERROR: The parsing table does not contain {} and {}", top, input_symbol);
//           return false;
//         }
//
//         // Get the production from the parsing table
//         const std::string production = this->parsing_table(top, input_symbol);
//
//         // Print production
//         fmt::print("Production: {}\n", production);
//
//         std::vector<std::string> symbols = pile::utils::split(production, " ");
//         symbols.erase(std::remove(symbols.begin(), symbols.end(), ""), symbols.end());
//         std::ranges::reverse(symbols);
//
//         // Pop top from the stack
//         stack.pop_back();
//
//         // Push the production to the stack
//         std::ranges::for_each(symbols, [&stack](const auto &symbol) {
//           if (symbol == Grammar::Empty{}.content) return;
//
//           stack.push_back(symbol);
//         });
//         fmt::print("\n");
//       }
//     } while (stack.back() != "$");
//
//     return true;
//   }
//
//   bool LL1::parse(const std::vector<Lexer::Token> &tokens) const {
//     fmt::print("{}\n", this->parsing_table);
//
//     std::vector<std::string> stack;
//     std::deque<Lexer::Token> input_tape(tokens.rbegin(), tokens.rend());
//     input_tape.push_back(Lexer::Token{.lexeme = "$"});
//
//     const auto first_production = fmt::format("<{}>", this->grammar.content[0].first.content);
//
//     stack.push_back("$");
//     stack.push_back(first_production);
//
//     do {
//       std::string top = stack.back();
//       std::string input_symbol
//           = input_tape[0].type == "keyword" ? input_tape[0].lexeme : input_tape[0].type;
//
//       // Copy the stack content minus the top
//       std::string stack_content;
//       std::ranges::for_each(
//           stack, [&stack_content](const auto &symbol) { stack_content += symbol + " "; });
//       stack_content = stack_content.substr(0, stack_content.find_last_of(" "));
//       stack_content = stack_content.substr(0, stack_content.find_last_of(" "));
//       stack_content.erase(std::remove(stack_content.begin(), stack_content.end(), ' '),
//                           stack_content.end());
//
//       std::string input_tape_content;
//       std::ranges::for_each(input_tape, [&input_tape_content](const auto &token) {
//         input_tape_content += token.lexeme + " ";
//       });
//       input_tape_content = input_tape_content.substr(0, input_tape_content.size() - 1);
//       input_tape_content = input_tape_content.substr(input_tape_content.find_first_of(" ") + 1);
//
//       // if input tape content is just $, then set it to empty
//       if (input_tape_content == "$") input_tape_content = "";
//
//       std::string print_top = fmt::format(
//           fmt::emphasis::underline | fmt::emphasis::bold | fg(fmt::color::gold), "{}", top);
//       std::string print_input_symbol
//           = fmt::format(fmt::emphasis::underline | fmt::emphasis::bold | fg(fmt::color::gold), "{}",
//                         input_symbol);
//
//       fmt::print("{}{}\t | {} {}\n", stack_content, print_top, print_input_symbol,
//                  input_tape_content);
//
//       // If top is a terminal
//       if (Grammar::is_terminal(top)) {
//         // If top is the same as the first character of the input tape
//         if (top != input_symbol) return false;
//
//         // Pop top from the stack
//         stack.pop_back();
//
//         // Remove the input symbol from the input tape
//         input_tape.pop_front();
//       } else {
//         // Remove the angle brackets from the production
//         // <a> => a
//         top = top.substr(1, top.size() - 2);
//
//         // If the parsing table has that production and terminal
//         if (!this->parsing_table.contains(top, input_symbol)) {
//           spdlog::error("ERROR: The parsing table does not contain {} and {}", top, input_symbol);
//           return false;
//         }
//
//         // Get the production from the parsing table
//         const std::string production = this->parsing_table(top, input_symbol);
//
//         // Print production
//         fmt::print("Production: {}\n", production);
//
//         std::vector<std::string> symbols = pile::utils::split(production, " ");
//         symbols.erase(std::remove(symbols.begin(), symbols.end(), ""), symbols.end());
//         std::ranges::reverse(symbols);
//
//         // Pop top from the stack
//         stack.pop_back();
//
//         // Push the production to the stack
//         std::ranges::for_each(symbols, [&stack](const auto &symbol) {
//           if (symbol == Grammar::Empty{}.content) return;
//
//           stack.push_back(symbol);
//         });
//         fmt::print("\n");
//       }
//     } while (stack.back() != "$");
//
//     return true;
//   }
// }  // namespace pile::Parser

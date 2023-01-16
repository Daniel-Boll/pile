#include <gtest/gtest.h>
#include <spdlog/fmt/fmt.h>

#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <stdexcept>

#include "pile/lexer/lexer.hpp"

using namespace pile::Parser;
using namespace pile::Lexer;
using namespace Grammar;

TEST(SLRParser, ShouldParseTest2) {
  try {
    auto grammar = parse<"assets/test/SLR_2.test.glc">({.expand = true});

    SLR parser{grammar};

    // String "(id)*id"
    std::vector<Token> tokens{
        Token{.file = "test", .type = "(", .lexeme = "(", .position = {1, 1}},
        Token{.file = "test", .type = "id", .lexeme = "id", .position = {1, 2}},
        Token{.file = "test", .type = ")", .lexeme = ")", .position = {1, 4}},
        Token{.file = "test", .type = "*", .lexeme = "*", .position = {1, 5}},
        Token{.file = "test", .type = "id", .lexeme = "id", .position = {1, 6}},
    };

    EXPECT_TRUE(parser.parse(tokens));
  } catch (...) {
    FAIL() << "Should not throw @ `ShouldCreateI0`";
  }
}

TEST(SLRParser, ShouldParseTest3) {
  try {
    auto grammar = parse<"assets/test/SLR_3.test.glc">({.expand = true});

    SLR parser{grammar};

    // String "[[a;a]]"
    std::vector<Token> tokens{
        Token{.type = "[", .lexeme = "[", .position = {1, 1}},
        Token{.type = "[", .lexeme = "[", .position = {1, 2}},
        Token{.type = "a", .lexeme = "a", .position = {1, 3}},
        Token{.type = ";", .lexeme = ";", .position = {1, 4}},
        Token{.type = "a", .lexeme = "a", .position = {1, 5}},
        Token{.type = "]", .lexeme = "]", .position = {1, 6}},
        Token{.type = "]", .lexeme = "]", .position = {1, 7}},
    };

    // std::vector<Token> tokens{
    //     Token{.type = "[", .lexeme = "[", .position = {1, 1}},
    //     Token{.type = "a", .lexeme = "a", .position = {1, 2}},
    //     Token{.type = ";", .lexeme = ";", .position = {1, 3}},
    //     Token{.type = "]", .lexeme = "]", .position = {1, 4}},
    // };

    EXPECT_TRUE(parser.parse(tokens));
  } catch (...) {
    FAIL() << "Should not throw @ `ShouldCreateI0`";
  }
}

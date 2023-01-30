#include <gtest/gtest.h>
#include <spdlog/fmt/fmt.h>

#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <stdexcept>

#include "pile/lexer/lexer.hpp"

using namespace pile::Parser;
using namespace pile::Lexer;
using namespace Grammar;

// Share the same parser accross all test suits.
class SLRParserFixture : public ::testing::Test {
protected:
  void SetUp() override {
    try {
      auto grammar = parse<"assets/grammar.spec.glc">({.expand = true});
      parser = std::make_unique<SLR>(grammar);
    } catch (...) {
      FAIL() << "Should not throw @ `SetUp`";
    }
  }

  std::unique_ptr<SLR> parser;
};

TEST_F(SLRParserFixture, ShouldParseNumbers) {
  try {
    std::vector<std::string> include_dir{"."};
    EXPECT_TRUE(parser->parse(
        pile::Lexer::analyze(fmt::format("example/{}.pile", "numbers"), &include_dir).tokens));
  } catch (...) {
    FAIL() << "Should not throw @ `ShouldParseNumbers`";
  }
}

// TODO: Share the same parser accross all test suits.
// TEST(SLRParser, ShouldParseNumbers) {
//   try {
//     // auto example = "numbers";
//     auto grammar = parse<"assets/grammar.spec.glc">({.expand = true});
//     SLR parser{grammar};
//
//     std::vector<std::string> include_dir{"."};
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "numbers"), &include_dir).tokens));
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "macros"), &include_dir).tokens));
//     // FIXME: empty program should be a valid program.
//     // EXPECT_TRUE(parser.parse(
//     //     pile::Lexer::analyze(fmt::format("example/{}.pile", "comment"),
//     &include_dir).tokens));
//
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "if-statement"),
//         &include_dir).tokens));
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "range"), &include_dir).tokens));
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "simple"), &include_dir).tokens));
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "string"), &include_dir).tokens));
//     EXPECT_TRUE(parser.parse(
//         pile::Lexer::analyze(fmt::format("example/{}.pile", "variable"), &include_dir).tokens));
//   } catch (...) {
//     FAIL() << "Should not throw @ `ShouldParseNumbers`";
//   }
// }

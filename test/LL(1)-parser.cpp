#include <gtest/gtest.h>

#include <pile/lexer/lexer.hpp>
#include <pile/parser/LL(1)/parser.hpp>

// TEST(LL_1_parser, ShouldAcceptNumericLiteral) {
//   using namespace pile::Parser;
//
//   const auto grammar = Grammar::parse<"assets/test/ll_one_1.test.glc">();
//   LL1 parser(grammar);
//
//   EXPECT_TRUE(parser.parse("id V id ∧ id"));
//   EXPECT_TRUE(parser.parse("id"));
//   EXPECT_FALSE(parser.parse("id V"));
// }
//
// TEST(LL_1_parser, ShouldAcceptStatement) {
//   using namespace pile::Parser;
//
//   const auto grammar = Grammar::parse<"assets/test/ll_one_2.test.glc">();
//   LL1 parser(grammar);
//
//   EXPECT_TRUE(parser.parse("begin int id , id id = id [ id + id ] end"));
//   EXPECT_FALSE(parser.parse("begin int id , id id = id [ id + ] end"));
// }

TEST(LL1_parser_integration, PileStatement) {
  using namespace pile::Parser;

  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/simple.pile", &include_dir);

  const auto grammar = Grammar::parse<"assets/ll1-grammar.spec.glc">();
  LL1 parser(grammar);

  EXPECT_TRUE(parser.parse(res.tokens));
}

#include <gtest/gtest.h>

#include <pile/parser/LL(1)/parser.hpp>

TEST(LL_1_parser, ShouldAcceptNumericLiteral) {
  using namespace pile::Parser;

  const auto grammar = Grammar::parse<"assets/test/ll_one_1.test.glc">();
  LL1 parser(grammar);

  // const auto accept = parser.parse("1");
  //
  // EXPECT_TRUE(accept);
}

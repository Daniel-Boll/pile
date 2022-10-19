#include <gtest/gtest.h>

#include <pile/lexer/lexer.hpp>

TEST(lexer, lexer) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/test.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 6);
}

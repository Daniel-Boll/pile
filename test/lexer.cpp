#include <gtest/gtest.h>

#include <pile/lexer/lexer.hpp>

// TEST(lexer, ShouldLexComments) {
//   std::vector<std::string> include_dir{"."};
//   // TODO: capture this from flags like: "--keep-comments"
//   auto res = pile::Lexer::analyze("example/comment.pile", &include_dir);
//
//   ASSERT_EQ(res.tokens.size(), 1);
// }

TEST(lexer, ShouldLexNumbers) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/numbers.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 25);
}

TEST(lexer, ShouldLexMacros) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/macros.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 4);
}

TEST(lexer, ShouldLexIfStatement) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/if-statement.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 32);
}

TEST(lexer, ShouldLexRange) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/range.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 13);
}

TEST(lexer, ShouldLexString) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/string.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 1);
}

TEST(lexer, ShouldLexVariable) {
  std::vector<std::string> include_dir{"."};
  auto res = pile::Lexer::analyze("example/variable.pile", &include_dir);

  ASSERT_EQ(res.tokens.size(), 21);
}

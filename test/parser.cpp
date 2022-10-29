#include <gtest/gtest.h>
#include <spdlog/fmt/fmt.h>

#include <pile/parser/grammar.hpp>
#include <stdexcept>

TEST(parser, ShouldOpenFile) {
  using namespace pile::Parser;

  try {
    Grammar::parse<"assets/grammar.spec.glc">();
  } catch (...) {
    FAIL() << "Should not throw";
  }
}

TEST(parser, ShouldFailOpeningFile) {
  using namespace pile::Parser;

  try {
    Grammar::parse<"!assets/grammar.spec.glc">();
  } catch (std::string const &error) {
    EXPECT_EQ(error, fmt::format("Could not open file: {}", "!assets/grammar.spec.glc"));
  } catch (...) {
    FAIL() << "Expected std::string";
  }
}

TEST(parser, ShouldComputeGrammar) {
  using namespace pile::Parser::Grammar;

  try {
    // <S> -> <S> s | s
    auto grammar = parse<"assets/test/grammar_1.test.glc">();
    GrammarContent expected = GrammarContent{{
        {Production{"S"}, {Production{"S"}, Terminal{"s"}}},
        {Production{"S"}, {Terminal{"s"}}},
    }};

    EXPECT_TRUE(GrammarContent::equal(grammar.content, expected.content));

    // <S> -> <S> s | s | <S> <B> ϵ
    grammar = parse<"assets/test/grammar_2.test.glc">();
    expected = GrammarContent{{
        {Production{"S"}, {Production{"S"}, Terminal{"s"}}},
        {Production{"S"}, {Terminal{"s"}}},
        {Production{"S"}, {Production{"S"}, Production{"B"}, Empty{}}},
    }};

    EXPECT_TRUE(GrammarContent::equal(grammar.content, expected.content));
  } catch (...) {
    FAIL() << "Should not throw";
  }
}

TEST(parser, ShouldNotComputeGrammar) {
  using namespace pile::Parser::Grammar;

  try {
    // <S> -> <S> s | s
    auto grammar = parse<"assets/test/grammar_1.test.glc">();
    GrammarContent expected = GrammarContent{{
        {Production{"!S"}, {Production{"S"}, Terminal{"s"}}},
        {Production{"!S"}, {Terminal{"s"}}},
    }};

    EXPECT_FALSE(GrammarContent::equal(grammar.content, expected.content));

    // <S> -> <S> s | s | <S> <B> ϵ
    grammar = parse<"assets/test/grammar_2.test.glc">();
    expected = GrammarContent{{
        {Production{"S"}, {Production{"S"}, Terminal{"s"}}},
        {Production{"S"}, {Terminal{"s"}}},
        {Production{"S"}, {Production{"S"}, Production{"B"}, Terminal{"ε"}}},
    }};

    EXPECT_FALSE(GrammarContent::equal(grammar.content, expected.content));
  } catch (...) {
    FAIL() << "Should not throw";
  }
}


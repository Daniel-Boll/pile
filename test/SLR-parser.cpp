#include <gtest/gtest.h>
#include <spdlog/fmt/fmt.h>

#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <stdexcept>

TEST(SLRParser, ShouldCreateI0) {
  using namespace pile::Parser;
  using namespace Grammar;

  try {
    auto grammar = parse<"assets/test/SLR_2.test.glc">({.expand = true});

    SLR parser{grammar};

    parser.populate_state_machine();
  } catch (...) {
    FAIL() << "Should not throw @ `ShouldCreateI0`";
  }
}

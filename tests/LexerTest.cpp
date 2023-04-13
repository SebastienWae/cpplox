#include <gtest/gtest.h>

#include "../src/lib/Lexer.hpp"

int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }

  int m = std::numeric_limits<int>::max();
  int m2 = std::numeric_limits<int>::max();
  int a = m + m2;

  return result;
}

// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) { EXPECT_EQ(Factorial(0), 1); }

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}

TEST(One, Two) {
  ErrorReporter err;
  Lexer l{"", err};

  auto tokens = l.scanTokens();
  // REQUIRE(tokens.has_value());
  // CHECK(tokens.value().get().empty());
  // CHECK(!err.hasErrors());
}
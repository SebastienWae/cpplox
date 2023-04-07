#include <fuzztest/fuzztest.h>
#include <gtest/gtest.h>

TEST(MyTestSuite, OnePlustTwoIsTwoPlusOne) { EXPECT_EQ(1 + 2, 2 + 1); }

void IntegerAdditionCommutes(int a, int b) {
  // EXPECT_EQ((long)a + (long)b, (long)b + (long)a);
  EXPECT_EQ(a + b, b + a);
}
FUZZ_TEST(MyTestSuite, IntegerAdditionCommutes);
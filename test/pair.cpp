
#include "ditto/pair.h"

#include <gtest/gtest.h>

#include <string>

using Ditto::Pair;

TEST(PairTest, Constructor) {
  Pair<int, std::string> pair{1, "cool!"};

  EXPECT_EQ(pair.left(), 1);
  EXPECT_STREQ(pair.right().c_str(), "cool!");

  pair.left() = 10;
  EXPECT_EQ(pair.left(), 10);
}

TEST(PairTest, ConstantPair) {
  const Pair<int, std::string> pair{1, "cool!"};

  EXPECT_EQ(pair.left(), 1);
  EXPECT_STREQ(pair.right().c_str(), "cool!");
}

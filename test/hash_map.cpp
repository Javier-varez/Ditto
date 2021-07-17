#include "ATE/hash_map.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ATE::Hash;
using ATE::HashMap;

TEST(HashMapTest, TinyMap) {
  HashMap<int, int> tiny_map{2};

  tiny_map[0] = 123;
  tiny_map[1234567890] = 234;
  tiny_map[1234567891] = 345;

  EXPECT_EQ(tiny_map[0], 123);
  EXPECT_EQ(tiny_map[1234567890], 234);
  EXPECT_EQ(tiny_map[1234567891], 345);

  // The first two elements should collide.
  EXPECT_EQ(Hash::calculate(0) % 2, 1);
  EXPECT_EQ(Hash::calculate(1234567890) % 2, 1);
  EXPECT_EQ(Hash::calculate(1234567891) % 2, 0);
}

TEST(HashMapTest, LargeMap) {
  HashMap<int, int> large_map{};

  large_map[0] = 123;
  large_map[1234567890] = 234;
  large_map[1234567891] = 345;

  EXPECT_EQ(large_map[0], 123);
  EXPECT_EQ(large_map[1234567890], 234);
  EXPECT_EQ(large_map[1234567891], 345);
}

TEST(HashMapTest, AtDoesNotInsertElement) {
  HashMap<int, int> large_map{};

  large_map[0] = 123;
  large_map[1234567890] = 234;
  large_map[1234567891] = 345;

  EXPECT_EQ(*large_map.at(0), 123);
  EXPECT_EQ(*large_map.at(1234567890), 234);
  EXPECT_EQ(*large_map.at(1234567891), 345);
  EXPECT_EQ(large_map.at(1234567892), nullptr);
  EXPECT_EQ(large_map[1234567892], 0);
  EXPECT_NE(large_map.at(1234567892), nullptr);
  EXPECT_EQ(*large_map.at(1234567892), 0);

  const HashMap<int, int>& const_map = large_map;
  EXPECT_NE(const_map.at(1234567892), nullptr);
}

TEST(HashMapTest, EraseElement) {
  HashMap<int, int> large_map{};

  large_map[0] = 123;
  large_map[1234567890] = 234;
  large_map[1234567891] = 345;

  EXPECT_NE(large_map.at(0), nullptr);
  large_map.erase(0);
  EXPECT_EQ(large_map.at(0), nullptr);
}

#include "linear_map.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(LinearMapTest, TinyMap) {
  LinearMap<int, int, 4> tiny_map;

  tiny_map[0] = 123;
  tiny_map[1234567890] = 234;
  tiny_map[1234567891] = 345;

  EXPECT_EQ(tiny_map[0], 123);
  EXPECT_EQ(tiny_map[1234567890], 234);
  EXPECT_EQ(tiny_map[1234567891], 345);
}

TEST(LinearMapTest, LargeMap) {
  LinearMap<int, int, 1024> large_map;

  large_map[0] = 123;
  large_map[1234567890] = 234;
  large_map[1234567891] = 345;

  EXPECT_EQ(large_map[0], 123);
  EXPECT_EQ(large_map[1234567890], 234);
  EXPECT_EQ(large_map[1234567891], 345);
}

TEST(LinearMapTest, AtDoesNotInsertElement) {
  LinearMap<int, int, 16> large_map{};

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

  const LinearMap<int, int, 16>& const_map = large_map;
  EXPECT_NE(const_map.at(1234567892), nullptr);
}

TEST(LinearMapTest, EraseElement) {
  LinearMap<int, int, 16> large_map{};

  large_map[0] = 123;
  large_map[1234567890] = 234;
  large_map[1234567891] = 345;

  EXPECT_NE(large_map.at(0), nullptr);
  large_map.erase(0);
  EXPECT_EQ(large_map.at(0), nullptr);
}

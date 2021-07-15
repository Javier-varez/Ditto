#include "linear_map.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ATE::LinearMap;

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

class Thing {
 public:
  explicit Thing(int i) : m_data(i) {}

  [[nodiscard]] auto value() -> int& { return m_data; }
  [[nodiscard]] auto value() const -> const int& { return m_data; }

  auto operator==(Thing other) const -> bool { return m_data == other.m_data; }

 private:
  int m_data;
};

TEST(LinearMapTest, NonDefaultConstructibleThing) {
  LinearMap<Thing, Thing, 16> large_map{};

  large_map[Thing{0}] = Thing{123};
  large_map[Thing{1234567890}] = Thing{234};
  large_map[Thing{1234567891}] = Thing{345};

  EXPECT_EQ(*large_map.at(Thing{0}), Thing{123});
  EXPECT_EQ(*large_map.at(Thing{1234567890}), Thing{234});
  EXPECT_EQ(*large_map.at(Thing{1234567891}), Thing{345});
}

TEST(LinearMapTest, Emplace) {
  LinearMap<std::string, Thing, 16> large_map{};

  large_map.emplace("0", 123);
  large_map.emplace("1", 234);
  large_map.emplace("1234", 345);

  EXPECT_EQ(*large_map.at("0"), Thing{123});
  EXPECT_EQ(*large_map.at("1"), Thing{234});
  EXPECT_EQ(*large_map.at("1234"), Thing{345});
}

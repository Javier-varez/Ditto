#include "ditto/fixed_flat_map.h"

#include <gtest/gtest.h>

using Ditto::FixedFlatMap;

TEST(FixedFlatMapTest, TinyMap) {
  FixedFlatMap<int, int, 32> tiny_map;

  ASSERT_TRUE(tiny_map.try_emplace(0, 123).is_ok());
  ASSERT_TRUE(tiny_map.try_emplace(1234567890, 234).is_ok());
  ASSERT_TRUE(tiny_map.try_emplace(1234567891, 345).is_ok());

  auto result = tiny_map[0];
  ASSERT_TRUE(result.is_ok());
  EXPECT_EQ(*DITTO_UNWRAP(result), 123);

  result = tiny_map[1234567890];
  ASSERT_TRUE(result.is_ok());
  EXPECT_EQ(*DITTO_UNWRAP(result), 234);

  result = tiny_map[1234567891];
  ASSERT_TRUE(result.is_ok());
  EXPECT_EQ(*DITTO_UNWRAP(result), 345);

  EXPECT_TRUE(tiny_map[1].is_error());
  EXPECT_TRUE(tiny_map[3234325].is_error());

  EXPECT_TRUE(tiny_map.try_emplace(1234567891, 345).is_error());
}

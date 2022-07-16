#include "ditto/fixed_vector.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

TEST(FixedVectorTest, Emplace) {
  static constexpr uint32_t LENGTH = 10;
  using FixedVector = Ditto::FixedVector<uint32_t, LENGTH>;
  FixedVector data;

  for (uint32_t i = 0; i < LENGTH; i++) {
    auto result = data.emplace(i);
    ASSERT_TRUE(result.is_ok());
    ASSERT_EQ(*result.ok_value(), i);
  }

  auto result = data.emplace(10u);
  ASSERT_TRUE(result.is_error());
  ASSERT_EQ(result.error_value(), FixedVector::Error::VectorIsFull);
}

TEST(FixedVectorTest, Push) {
  static constexpr uint32_t LENGTH = 10;
  using FixedVector = Ditto::FixedVector<uint32_t, LENGTH>;
  FixedVector data;

  for (uint32_t i = 0; i < LENGTH; i++) {
    auto result = data.push(i);
    ASSERT_TRUE(result.is_ok());
    ASSERT_EQ(*result.ok_value(), i);
  }

  auto result = data.push(10u);
  ASSERT_TRUE(result.is_error());
  ASSERT_EQ(result.error_value(), FixedVector::Error::VectorIsFull);
}

TEST(FixedVectorTest, SquareBracketOperator) {
  static constexpr uint32_t LENGTH = 10;
  using FixedVector = Ditto::FixedVector<uint32_t, LENGTH>;
  FixedVector data;

  EXPECT_TRUE(data.push(123).is_ok());

  auto result = data[0];
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(*result.ok_value(), 123);

  const auto& const_data = data;

  auto const_result = const_data[0];
  EXPECT_TRUE(const_result.is_ok());
  EXPECT_EQ(*const_result.ok_value(), 123);

  ASSERT_EQ(const_data.size(), 1);

  const_result = const_data[1];
  EXPECT_TRUE(const_result.is_error());
  EXPECT_EQ(const_result.error_value(), FixedVector::Error::IndexOutOfRange);
}

TEST(FixedVectorTest, IteratorWorks) {
  static constexpr uint32_t LENGTH = 10;
  using FixedVector = Ditto::FixedVector<uint32_t, LENGTH>;
  FixedVector data;

  for (auto i = 0; i < 10; i++) {
    ASSERT_TRUE(data.push(i).is_ok());
  }

  auto iter = data.begin();
  EXPECT_EQ(*(iter + 2), 2);
  iter += 2;
  EXPECT_EQ(*(iter), 2);
  EXPECT_EQ(iter[2], 4);

  EXPECT_EQ(iter - data.begin(), 2);
  auto difference = data.begin() - iter;
  EXPECT_EQ(difference, -2);
  EXPECT_EQ(*(data.begin() - difference), 2);

  auto const_iter = data.cbegin();
  EXPECT_EQ(*(const_iter + 2), 2);
  const_iter += 2;
  EXPECT_EQ(*(const_iter), 2);
  EXPECT_EQ(const_iter[2], 4);

  auto num_elements = 0;
  for (const auto& val : data) {
    EXPECT_EQ(val, num_elements);
    num_elements++;
  }
  EXPECT_EQ(num_elements, 10);
}

// Check that the iterator type is valid
static_assert(
    std::random_access_iterator<Ditto::FixedVector<int, 10>::Iterator<int>>);

#include "ditto/span.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(SpanTest, constructorFromPointerAndLength) {
  constexpr uint32_t DATA_LEN = 10;
  uint8_t buffer[DATA_LEN];
  Ditto::span<uint8_t> my_span{buffer, DATA_LEN};

  EXPECT_EQ(my_span.data(), buffer);
  EXPECT_EQ(my_span.size(), DATA_LEN);
}

TEST(SpanTest, constructorFromConstPointerAndLength) {
  constexpr uint32_t DATA_LEN = 10;
  const uint8_t buffer[DATA_LEN]{0, 1, 2};
  Ditto::span<const uint8_t> my_span{buffer, DATA_LEN};

  EXPECT_EQ(my_span.data(), buffer);
  EXPECT_EQ(my_span.size(), DATA_LEN);
}

TEST(SpanTest, constructorFromVector) {
  std::vector<uint8_t> data;
  Ditto::span my_span{data};

  EXPECT_EQ(my_span.data(), data.data());
  EXPECT_EQ(my_span.size(), data.size());
}

TEST(SpanTest, constructorFromConstVector) {
  const std::vector<uint8_t> data;
  Ditto::span<const uint8_t> my_span{data};

  EXPECT_EQ(my_span.data(), data.data());
  EXPECT_EQ(my_span.size(), data.size());

  const Ditto::span<const uint8_t> const_span{data};

  EXPECT_EQ(const_span.data(), data.data());
  EXPECT_EQ(const_span.size(), data.size());
}

TEST(SpanTest, constructorFromArray) {
  std::array<uint8_t, 10> data;
  Ditto::span my_span{data};

  EXPECT_EQ(my_span.data(), data.data());
  EXPECT_EQ(my_span.size(), data.size());

  const Ditto::span const_span{data};

  EXPECT_EQ(const_span.data(), data.data());
  EXPECT_EQ(const_span.size(), data.size());
}

TEST(SpanTest, constructorFromConstArray) {
  const std::array<uint8_t, 10> data{0, 1, 2};
  Ditto::span<const uint8_t> my_span{data};

  EXPECT_EQ(my_span.data(), data.data());
  EXPECT_EQ(my_span.size(), data.size());

  const Ditto::span<const uint8_t> const_span{data};

  EXPECT_EQ(const_span.data(), data.data());
  EXPECT_EQ(const_span.size(), data.size());
}

TEST(SpanTest, constructFromAnotherSpan) {
  std::array<uint8_t, 10> data{0, 1, 2};
  Ditto::span<uint8_t> my_span{data};
  Ditto::span<uint8_t> other_span{my_span};

  EXPECT_EQ(other_span.data(), data.data());
  EXPECT_EQ(other_span.size(), data.size());
}

TEST(SpanTest, copyAssignmentOperator) {
  std::array<uint8_t, 10> data{0, 1, 2};
  std::array<uint8_t, 10> data2{0, 1, 2};
  Ditto::span<uint8_t> my_span{data};
  const Ditto::span<uint8_t> other_span{data2};

  my_span = other_span;

  EXPECT_EQ(my_span.data(), data2.data());
  EXPECT_EQ(my_span.size(), data2.size());
}

TEST(SpanTest, operatorSquareBrackets) {
  std::array<uint8_t, 10> data{123, 8, 9};
  Ditto::span my_span{data};

  EXPECT_EQ(my_span[0], 123);
  EXPECT_EQ(my_span[1], 8);
  EXPECT_EQ(my_span[2], 9);

  my_span[2] = 12;

  const Ditto::span const_span{data};

  EXPECT_EQ(const_span[0], 123);
  EXPECT_EQ(const_span[1], 8);
  EXPECT_EQ(const_span[2], 12);
}

TEST(SpanTest, getFrontAndBackElement) {
  std::array<uint8_t, 3> data{123, 8, 9};
  Ditto::span my_span{data};

  EXPECT_EQ(my_span.front(), 123);
  EXPECT_EQ(my_span.back(), 9);
}

TEST(SpanTest, getSize) {
  std::array<uint32_t, 123> data{123, 8, 9};
  Ditto::span my_span{data};

  EXPECT_EQ(my_span.size(), 123);
  EXPECT_EQ(my_span.size_bytes(), 123 * 4);
}

TEST(SpanTest, isEmpty) {
  std::vector<uint32_t> data;
  Ditto::span my_span{data};

  EXPECT_TRUE(my_span.empty());

  data.push_back(0);
  Ditto::span new_span{data};

  EXPECT_FALSE(new_span.empty());
}

TEST(SpanTest, getSubspanFromFirst) {
  std::array<uint32_t, 10> data{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Ditto::span my_span{data};

  auto first = my_span.first(5);
  EXPECT_EQ(first.data(), data.data());
  EXPECT_EQ(first.size(), 5);
}

TEST(SpanTest, getSubspanFromLast) {
  std::array<uint32_t, 10> data{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Ditto::span my_span{data};

  auto last = my_span.last(2);
  EXPECT_EQ(last.data(), data.data() + 8);
  EXPECT_EQ(last.size(), 2);

  EXPECT_EQ(last[0], 8);
}

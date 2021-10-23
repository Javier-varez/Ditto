
#include "ditto/fixed.h"

#include <gtest/gtest.h>

#include <memory>

using namespace Ditto::numeric_literals;

using Ditto::FP16;
using Ditto::FP32;
using Ditto::FP8;

TEST(FixedPointTest, ConstructFromDouble) {
  FP8<8> fp = 0.5_uq0_8;
  EXPECT_NEAR(fp.as_double(), 0.5, 1 / 256.0);

  FP32<32> fp2 = 0.2333_uq0_32;
  EXPECT_NEAR(fp2.as_double(), 0.2333, 1 / double{1ULL << 32});
}

TEST(FixedPointTest, ConstructFromUnderlying) {
  FP8<8> fp = FP8<8>::from_raw(std::uint8_t{128U});

  EXPECT_NEAR(fp.as_double(), 0.5, 1 / 256.0);
}

TEST(FixedPointTest, Addition) {
  FP8<8> fp = FP8<8>{0.5_uq0_8 + 0.12_uq0_8};

  EXPECT_NEAR(fp.as_double(), 0.62, 1 / 256.0);
}

TEST(FixedPointTest, Subtraction) {
  FP8<8> fp = FP8<8>{0.5_uq0_8 - 0.12_uq0_8};

  EXPECT_NEAR(fp.as_double(), 0.38, 1 / 256.0);
}

TEST(FixedPointTest, Multiplication) {
  FP16<16> fp = FP16<16>{0.5_uq0_8 * 0.12_uq0_8};

  EXPECT_NEAR(fp.as_double(), 0.06, 1 / double{1ULL << 8});
}

TEST(FixedPointTest, Division) {
  FP32<16> num = FP32<16>{0.5_uq0_32};
  FP8<8> den = 0.12_uq0_8;
  FP32<8> fp = num / den;

  EXPECT_NEAR(fp.as_double(), 4.266666, 1 / double{1ULL << 8});
}

TEST(FixedPointTest, FromInteger) {
  auto denominator = 20.0_uq8_8;
  auto numerator = FP32<17>::from_integer(750U);

  FP32<9> result = numerator / denominator;
  EXPECT_NEAR(result.as_double(), 37.5, 1 / double{1ULL << 9});
}

TEST(FixedPointTest, ComparisonOperators) {
  auto val = FP32<9>::from_floating_point(37.5);
  EXPECT_FALSE(val < FP32<9>::from_floating_point(37.4));
  EXPECT_TRUE(val > FP32<9>::from_floating_point(37.4));
  EXPECT_TRUE(val < FP32<9>::from_floating_point(37.6));
  EXPECT_FALSE(val > FP32<9>::from_floating_point(37.6));

  EXPECT_TRUE(val >= FP32<9>::from_floating_point(37.5));
  EXPECT_TRUE(val <= FP32<9>::from_floating_point(37.5));
  EXPECT_FALSE(val > FP32<9>::from_floating_point(37.5));
  EXPECT_FALSE(val < FP32<9>::from_floating_point(37.5));
  EXPECT_TRUE(val == FP32<9>::from_floating_point(37.5));
  EXPECT_FALSE(val == FP32<9>::from_floating_point(37.6));

  EXPECT_FALSE(val == 37);
  EXPECT_TRUE(val >= 37);
  EXPECT_TRUE(val <= 38);
  EXPECT_FALSE(val >= 38);
  EXPECT_FALSE(val <= 37);
}

TEST(FixedPointTest, FP64_construct_from_integer) {
  constexpr uint32_t value = 0xFFFFFFFF;
  Ditto::FP64<16> fp64 = Ditto::FP64<16>::from_integer(value);
  EXPECT_EQ(fp64.as_integer(), 0xFFFFFFFF);
}

TEST(FixedPointTest, FP64_addition_with_integer) {
  constexpr uint32_t value = 0xFFFFFFFF;
  Ditto::FP64<16> fp64 = Ditto::FP64<16>::from_integer(value);

  auto result = fp64 + value;
  EXPECT_EQ(result.as_integer(), 0x1FFFFFFFEULL);

  result += value;
  EXPECT_EQ(result.as_integer(), 0x2FFFFFFFDULL);
}

TEST(FixedPointTest, FP64_subtraction_with_integer) {
  constexpr uint32_t value = 0xFFFFFFFF;
  Ditto::FP64<16> fp64 = Ditto::FP64<16>::from_integer(0xFFFFFFFFFULL);

  auto result = fp64 - value;
  EXPECT_EQ(result.as_integer(), 0xF00000000ULL);

  result -= value;
  EXPECT_EQ(result.as_integer(), 0xE00000001ULL);
}

TEST(FixedPointTest, FP64_multiplication_with_integer) {
  constexpr uint32_t value = 0xFFFFFFFFULL;
  Ditto::FP64<16> fp64 = Ditto::FP64<16>::from_integer(1234);

  auto result = fp64 * value;
  EXPECT_EQ(result.as_integer(), 0x4D1FFFFFB2EULL);

  result *= 2;
  EXPECT_EQ(result.as_integer(), 0x4D1FFFFFB2EULL * 2);
}

TEST(FixedPointTest, FP64_division_with_integer) {
  constexpr uint32_t value = 1234;
  Ditto::FP64<16> fp64 = Ditto::FP64<16>::from_integer(0xFFFFFFFFFFULL);

  auto result = fp64 / value;
  EXPECT_EQ(result.as_integer(), 0x351BCC8DULL);

  result /= 2;
  EXPECT_EQ(result.as_integer(), 0x351BCC8DULL / 2);
}

TEST(FixedPointTest, round) {
  FP32<8> val = FP32<8>::from_floating_point(10.5);
  EXPECT_EQ(val.round().as_integer(), 11);
  EXPECT_EQ(val.as_integer(), 10);

  val = FP32<8>::from_floating_point(10.49);
  EXPECT_EQ(val.round().as_integer(), 10);
  EXPECT_EQ(val.as_integer(), 10);

  val = FP32<8>::from_floating_point(10.51);
  EXPECT_EQ(val.round().as_integer(), 11);
  EXPECT_EQ(val.as_integer(), 10);
}

TEST(FixedPointTest, add_oposite_order) {
  FP32<12> number = FP32<12>::from_floating_point(123.0);
  const auto result = 123 + number;
  EXPECT_EQ(result.as_integer(), 246);
}

TEST(FixedPointTest, sub_oposite_order) {
  FP32<12> number = FP32<12>::from_floating_point(123.0);
  const auto result = 123 - number;
  EXPECT_EQ(result.as_integer(), 0);
}

TEST(FixedPointTest, mul_oposite_order) {
  FP32<12> number = FP32<12>::from_floating_point(123.0);
  const auto result = 2 * number;
  EXPECT_EQ(result.as_integer(), 246);
}

TEST(FixedPointTest, div_oposite_order) {
  FP32<12> number = FP32<12>::from_floating_point(123.0);
  const auto result = 123 / number;
  EXPECT_EQ(result.as_integer(), 1);
}

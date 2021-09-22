
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
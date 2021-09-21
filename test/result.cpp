
#include "ditto/result.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "assert_mock.h"

using Ditto::Result;
using testing::_;
using testing::Mock;
using testing::StrictMock;

TEST(ResultTest, int_ok_void_err) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  Result<int, void> result = Result<int, void>::ok(124);

  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 124);
  EXPECT_FALSE(result.is_error());

  result = Result<int, void>::error();
  EXPECT_FALSE(result.is_ok());
  EXPECT_TRUE(result.is_error());

  EXPECT_CALL(assert, assert_failed(_, _, _));
  auto& ok_val = result.ok_value();
  Mock::VerifyAndClearExpectations(&assert);

  Ditto::g_assert = nullptr;
}

enum class Error { SOME_ERROR, SOME_OTHER_ERROR };

TEST(ResultTest, int_ok_enum_err) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto result = Result<int, Error>::ok(124);

  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 124);
  EXPECT_FALSE(result.is_error());

  result = Result<int, Error>::error(Error::SOME_ERROR);
  EXPECT_FALSE(result.is_ok());
  EXPECT_TRUE(result.is_error());

  EXPECT_EQ(Error::SOME_ERROR, result.error_value());

  EXPECT_CALL(assert, assert_failed(_, _, _));
  auto& ok_val = result.ok_value();
  Mock::VerifyAndClearExpectations(&assert);

  Ditto::g_assert = nullptr;
}

TEST(ResultTest, void_ok_enum_err) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto result = Result<void, Error>::ok();

  EXPECT_TRUE(result.is_ok());
  EXPECT_FALSE(result.is_error());

  EXPECT_CALL(assert, assert_failed(_, _, _));
  auto& err_val = result.error_value();
  Mock::VerifyAndClearExpectations(&assert);

  result = Result<void, Error>::error(Error::SOME_ERROR);
  EXPECT_FALSE(result.is_ok());
  EXPECT_TRUE(result.is_error());

  EXPECT_EQ(Error::SOME_ERROR, result.error_value());
  Ditto::g_assert = nullptr;
}

TEST(ResultTest, propagater_error) {
  auto divide_and_add = [](uint32_t numerator, uint32_t denominator,
                           uint32_t add) -> Result<double, const char*> {
    auto division = [](uint32_t numerator,
                       uint32_t denominator) -> Result<uint32_t, const char*> {
      if (denominator == 0) {
        return Result<uint32_t, const char*>::error(
            "Can't divide by 0, sorry :-(");
      }
      return Result<uint32_t, const char*>::ok(numerator / denominator);
    };

    uint32_t division_result = PROPAGATE(division(numerator, denominator));

    return Result<double, const char*>::ok(
        static_cast<double>(division_result + add));
  };

  auto result = divide_and_add(12, 0, 1);
  EXPECT_TRUE(result.is_error());
  EXPECT_STREQ(result.error_value(), "Can't divide by 0, sorry :-(");

  result = divide_and_add(12, 2, 1);
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 7);
}

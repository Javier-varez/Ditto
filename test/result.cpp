#include "ditto/result.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
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

TEST(ResultTest, verify_ok) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto returns_result = []() -> Result<void, uint32_t> {
    return Result<void, uint32_t>::ok();
  };
  DITTO_VERIFY_OK(returns_result());

  EXPECT_CALL(assert, assert_failed(_, _, _));
  DITTO_VERIFY_ERR(returns_result());

  Ditto::g_assert = nullptr;
}

TEST(ResultTest, verify_err) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto returns_result = []() -> Result<void, uint32_t> {
    return Result<void, uint32_t>::error(123u);
  };
  DITTO_VERIFY_ERR(returns_result());

  EXPECT_CALL(assert, assert_failed(_, _, _));
  DITTO_VERIFY_OK(returns_result());

  Ditto::g_assert = nullptr;
}

TEST(ResultTest, propagate_error) {
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

    uint32_t division_result =
        DITTO_PROPAGATE(division(numerator, denominator));

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

TEST(ResultTest, propagate_ok) {
  enum class InnerError { DivideByZero, Other };

  enum class OuterError { DivisionError, Unknown };
  auto wrapped_division =
      [](uint32_t numerator,
         uint32_t denominator) -> Result<uint32_t, OuterError> {
    auto division = [](uint32_t numerator,
                       uint32_t denominator) -> Result<uint32_t, InnerError> {
      if (denominator == 0) {
        return Result<uint32_t, InnerError>::error(InnerError::DivideByZero);
      }
      return Result<uint32_t, InnerError>::ok(numerator / denominator);
    };

    const InnerError error =
        DITTO_PROPAGATE_OK(division(numerator, denominator));

    switch (error) {
      case InnerError::DivideByZero:
        return Result<uint32_t, OuterError>::error(OuterError::DivisionError);
      case InnerError::Other:
      default:
        return Result<uint32_t, OuterError>::error(OuterError::Unknown);
    }
  };

  auto result = wrapped_division(12, 0);
  EXPECT_TRUE(result.is_error());
  EXPECT_EQ(result.error_value(), OuterError::DivisionError);

  result = wrapped_division(12, 2);
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 6);
}

TEST(ResultTest, unwrap) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto result = Result<uint32_t, const char*>::ok(123U);
  EXPECT_EQ(DITTO_UNWRAP(result), 123);

  result = Result<uint32_t, const char*>::error("");
  EXPECT_CALL(assert, assert_failed(_, _, _)).Times(2);
  DITTO_UNWRAP(result);

  Ditto::g_assert = nullptr;
}

TEST(ResultTest, unwrap_or) {
  auto result = Result<uint32_t, const char*>::ok(123U);
  EXPECT_EQ(DITTO_UNWRAP_OR(result, 0U), 123);

  result = Result<uint32_t, const char*>::error("");
  EXPECT_EQ(DITTO_UNWRAP_OR(result, 0U), 0);
}

TEST(ResultTest, unwrap_or_else) {
  uint32_t i = 0;
  auto else_functor = [&]() { return i++; };

  auto result = Result<uint32_t, const char*>::ok(123U);
  EXPECT_EQ(DITTO_UNWRAP_OR_ELSE(result, else_functor), 123);

  result = Result<uint32_t, const char*>::error("");
  EXPECT_EQ(DITTO_UNWRAP_OR_ELSE(result, else_functor), 0);
  EXPECT_EQ(DITTO_UNWRAP_OR_ELSE(result, else_functor), 1);
}

TEST(ResultTest, unwrap_err) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;

  auto result = Result<const char*, uint32_t>::ok("");
  EXPECT_CALL(assert, assert_failed(_, _, _)).Times(2);
  DITTO_UNWRAP_ERR(result);

  result = Result<const char*, uint32_t>::error(123U);
  EXPECT_EQ(DITTO_UNWRAP_ERR(result), 123);

  Ditto::g_assert = nullptr;
}

TEST(ResultTest, unwrap_err_or) {
  auto result = Result<uint32_t, const char*>::ok(123U);
  EXPECT_STREQ(DITTO_UNWRAP_ERR_OR(result, "alternative"), "alternative");

  result = Result<uint32_t, const char*>::error("");
  EXPECT_STREQ(DITTO_UNWRAP_ERR_OR(result, "alternative"), "");
}

TEST(ResultTest, unwrap_err_or_else) {
  uint32_t i = 0;
  auto else_functor = [&]() { return i++; };

  auto result = Result<const char*, uint32_t>::error(123U);
  EXPECT_EQ(DITTO_UNWRAP_ERR_OR_ELSE(result, else_functor), 123);

  result = Result<const char*, uint32_t>::ok("");
  EXPECT_EQ(DITTO_UNWRAP_ERR_OR_ELSE(result, else_functor), 0);
  EXPECT_EQ(DITTO_UNWRAP_ERR_OR_ELSE(result, else_functor), 1);
}

TEST(ResultTest, SameTypes) {
  auto divide_and_add = [](uint32_t numerator, uint32_t denominator,
                           uint32_t add) -> Result<double, uint32_t> {
    auto division = [](uint32_t numerator,
                       uint32_t denominator) -> Result<uint32_t, uint32_t> {
      if (denominator == 0) {
        return Result<uint32_t, uint32_t>::error(0xFFFF5A5Au);
      }
      return Result<uint32_t, uint32_t>::ok(numerator / denominator);
    };

    uint32_t division_result =
        DITTO_PROPAGATE(division(numerator, denominator));

    return Result<double, uint32_t>::ok(
        static_cast<double>(division_result + add));
  };

  auto result = divide_and_add(12, 0, 1);
  EXPECT_TRUE(result.is_error());
  EXPECT_EQ(result.error_value(), 0xFFFF5A5Au);

  result = divide_and_add(12, 2, 1);
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 7);
}

TEST(ResultTest, MapOk) {
  const auto map_func = [](uint32_t val) -> double {
    return static_cast<double>(val);
  };
  Result result = Result<uint32_t, bool>::ok(123u).map_ok(map_func);
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(static_cast<uint32_t>(result.ok_value()), 123u);

  result = Result<uint32_t, bool>::error(false).map_ok(map_func);
  EXPECT_TRUE(result.is_error());
  EXPECT_FALSE(result.error_value());
  result = Result<uint32_t, bool>::error(true).map_ok(map_func);
  EXPECT_TRUE(result.is_error());
  EXPECT_TRUE(result.error_value());
}

TEST(ResultTest, MapErr) {
  const auto map_func = [](bool val) -> const char* {
    if (val) {
      return "true";
    }
    return "false";
  };

  Result result = Result<uint32_t, bool>::error(false).map_err(map_func);
  EXPECT_TRUE(result.is_error());
  EXPECT_STREQ(result.error_value(), "false");

  result = Result<uint32_t, bool>::error(true).map_err(map_func);
  EXPECT_TRUE(result.is_error());
  EXPECT_STREQ(result.error_value(), "true");

  result = Result<uint32_t, bool>::ok(1234u).map_err(map_func);
  EXPECT_TRUE(result.is_ok());
  EXPECT_EQ(result.ok_value(), 1234u);
}

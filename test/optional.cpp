
#include "ditto/optional.h"

#include <gtest/gtest.h>

#include <string>

TEST(OptionalTest, default_constructor) { Ditto::optional<int> optional; }

TEST(OptionalTest, construct_from_value) { Ditto::optional<int> optional{83}; }

TEST(OptionalTest, has_value) {
  constexpr Ditto::optional<int> empty_optional;
  constexpr Ditto::optional<int> valid_optional{83};
  static_assert(!empty_optional.has_value(), "Empty optional has value!");
  static_assert(valid_optional.has_value(),
                "Valid optional does not have value!");
  EXPECT_FALSE(empty_optional.has_value());
  EXPECT_TRUE(valid_optional.has_value());
}

TEST(OptionalTest, construct_from_rvalue) {
  constexpr Ditto::optional<int> int_opt{Ditto::optional<char>{'3'}.value()};
  EXPECT_EQ(int_opt.value(), '3');
}

TEST(OptionalTest, copy_assignment_from_optional) {
  constexpr Ditto::optional<int> int_opt1{2};
  Ditto::optional<int> int_opt2{};
  int_opt2 = int_opt1;
  EXPECT_EQ(int_opt2.value(), 2);
  int_opt2 = Ditto::optional<int>{3};
  EXPECT_EQ(int_opt2.value(), 3);

  Ditto::optional<std::string> str_opt1{"hello"};
  Ditto::optional<std::string> str_opt2;
  str_opt2 = str_opt1;
}

TEST(OptionalTest, copy_assignment_from_underlying) {
  Ditto::optional<int> int_opt2{};
  int_opt2 = 3;
  EXPECT_EQ(int_opt2.value(), 3);
}

struct NonDefaultConstructible {
  constexpr explicit NonDefaultConstructible(int) {}
};

TEST(OptionalTest, non_default_constructible_optional) {
  constexpr Ditto::optional<NonDefaultConstructible> opt;
  Ditto::optional<NonDefaultConstructible> opt2;
}

TEST(OptionalTest, get_value_from_lvalue_optional) {
  constexpr Ditto::optional<int> optional{83};
  static_assert(optional.value() == 83, "Value is not 83");
  EXPECT_EQ(optional.value(), 83);
}

TEST(OptionalTest, get_value_from_rvalue_optional) {
  EXPECT_EQ(Ditto::optional<int>{83}.value(), 83);
}

TEST(OptionalTest, get_value_or_default) {
  Ditto::optional<int> opt;
  EXPECT_EQ(opt.value_or(12), 12);
  opt = Ditto::optional<int>{1};
  EXPECT_EQ(opt.value_or(12), 1);
}

TEST(OptionalTest, get_value_or_default_from_rvalue_optional) {
  EXPECT_EQ(Ditto::optional<int>{1}.value_or(12), 1);
  EXPECT_EQ(Ditto::optional<int>{}.value_or(12), 12);
}

TEST(OptionalTest, reset) {
  Ditto::optional<int> opt{3};
  opt.reset();
  EXPECT_FALSE(opt);
}

TEST(OptionalTest, arrow_operator) {
  Ditto::optional<std::string> opt{"hello"};
  EXPECT_EQ(opt->length(), 5);

  EXPECT_EQ(
      Ditto::optional<std::string> { "hi" }->length(), 2);

  const Ditto::optional<std::string> const_opt{"hello"};
  EXPECT_EQ(const_opt->length(), 5);
}

TEST(OptionalTest, star_operator) {
  Ditto::optional<std::string> opt{"hello"};
  EXPECT_EQ((*opt).length(), 5);

  EXPECT_EQ((*Ditto::optional<std::string>{"hi"}).length(), 2);

  const Ditto::optional<std::string> const_opt{"hello"};
  EXPECT_EQ((*const_opt).length(), 5);
  EXPECT_EQ((*std::move(const_opt)).length(), 5);
}

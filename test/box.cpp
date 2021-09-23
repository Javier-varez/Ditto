
#include "ditto/box.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using Ditto::Box;
using testing::ElementsAreArray;

TEST(BoxTest, DefaultConstructor) {
  Box<int> box;
  EXPECT_NE(box.get(), nullptr);
}

TEST(BoxTest, ForwardingConstructor) {
  auto box = Box<std::string>{"Hi there!"};
  EXPECT_NE(box.get(), nullptr);
  EXPECT_STREQ(box->c_str(), "Hi there!");
}

TEST(BoxTest, MoveConstructor) {
  auto box = Box<int>{1};
  const int* const first_ptr = box.get();

  auto another_box{std::move(box)};
  EXPECT_NE(box.get(), first_ptr);
  EXPECT_NE(box.get(), nullptr);
  EXPECT_EQ(another_box.get(), first_ptr);
  EXPECT_EQ(*another_box, 1);
}

TEST(BoxTest, MoveAssignment) {
  auto box = Box<int>{1};
  auto another_box = Box<int>{7};

  const int* const first_ptr = box.get();
  const int* const prev_ptr = another_box.get();

  another_box = {std::move(box)};
  EXPECT_NE(box.get(), nullptr);
  EXPECT_NE(box.get(), first_ptr);
  EXPECT_EQ(another_box.get(), first_ptr);
  EXPECT_EQ(*another_box, 1);
}

TEST(BoxTest, Leak) {
  auto box = Box<int>{1};
  const int* const first_ptr = box.get();
  const int* const released_ptr = box.leak();

  EXPECT_NE(box.get(), nullptr);
  EXPECT_NE(box.get(), first_ptr);
  EXPECT_EQ(released_ptr, first_ptr);

  // Delete the leaked pointer
  delete released_ptr;
}

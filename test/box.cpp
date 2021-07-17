
#include "ATE/box.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ATE::Box;
using ATE::make_box;
using testing::ElementsAreArray;

TEST(BoxTest, DefaultConstructor) {
  Box<int> box;
  EXPECT_FALSE(box);
  EXPECT_EQ(box.get(), nullptr);
}

TEST(BoxTest, ValidBox) {
  auto box = make_box<int>(1);
  EXPECT_TRUE(box);
  EXPECT_NE(box.get(), nullptr);
  EXPECT_EQ(*box, 1);
}

class Dummy {
 public:
  explicit Dummy(int value) : m_value(value) {}
  [[nodiscard]] auto value() const -> int { return m_value; }

 private:
  int m_value;
};

TEST(BoxTest, ContainedClass) {
  auto box = make_box<Dummy>(1);
  EXPECT_TRUE(box);
  EXPECT_NE(box.get(), nullptr);
  EXPECT_EQ(box->value(), 1);
}

TEST(BoxTest, MoveConstructor) {
  auto box = make_box<Dummy>(1);
  const Dummy* const first_ptr = box.get();

  auto another_box{std::move(box)};
  EXPECT_FALSE(box);
  EXPECT_EQ(box.get(), nullptr);
  EXPECT_TRUE(another_box);
  EXPECT_NE(another_box.get(), nullptr);
  EXPECT_EQ(another_box.get(), first_ptr);
  EXPECT_EQ(another_box->value(), 1);
}

TEST(BoxTest, MoveAssignment) {
  auto box = make_box<Dummy>(1);
  Box<Dummy> another_box = make_box<Dummy>(7);

  const Dummy* const first_ptr = box.get();
  const Dummy* const prev_ptr = another_box.get();

  another_box = {std::move(box)};
  EXPECT_FALSE(box);
  EXPECT_EQ(box.get(), nullptr);
  EXPECT_TRUE(another_box);
  EXPECT_NE(another_box.get(), nullptr);
  EXPECT_NE(another_box.get(), prev_ptr);
  EXPECT_EQ(another_box.get(), first_ptr);
  EXPECT_EQ(another_box->value(), 1);

  another_box = {std::move(another_box)};
}

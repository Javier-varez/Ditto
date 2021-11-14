#include "ditto/binary_search_tree.h"

#include <gtest/gtest.h>

TEST(BinarySearchTreeTest, Constructor) {
  Ditto::BinarySearchTree<int, int> tree;
}

TEST(BinarySearchTreeTest, InsertSingle) {
  Ditto::BinarySearchTree<int, int> tree;
  tree.insert(12, 234);
}

TEST(BinarySearchTreeTest, InsertMultiple) {
  Ditto::BinarySearchTree<int, int> tree;
  tree.insert(12, 234);
  tree.insert(23, 235);
  tree.insert(25, 236);
  tree.insert(11, 237);
}

TEST(BinarySearchTreeTest, Lookup) {
  Ditto::BinarySearchTree<int, int> tree;
  tree.insert(12, 234);
  tree.insert(23, 235);
  tree.insert(25, 236);
  tree.insert(11, 237);

  auto result = tree.lookup(12);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result.value(), 234);

  result = tree.lookup(23);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result.value(), 235);

  result = tree.lookup(25);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result.value(), 236);

  result = tree.lookup(11);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result.value(), 237);

  result = tree.lookup(10);
  ASSERT_FALSE(result.has_value());
}

TEST(BinarySearchTreeTest, Erase) {
  Ditto::BinarySearchTree<int, int> tree;
  tree.insert(12, 234);
  tree.insert(23, 235);
  tree.insert(25, 236);
  tree.insert(26, 237);
  tree.insert(21, 238);
  tree.insert(22, 239);
  tree.insert(11, 240);

  auto result = tree.erase(23);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 235);

  auto look_result = tree.lookup(23);
  ASSERT_FALSE(look_result.has_value());

  look_result = tree.lookup(12);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 234);

  look_result = tree.lookup(25);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 236);

  look_result = tree.lookup(26);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 237);

  look_result = tree.lookup(21);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 238);

  look_result = tree.lookup(22);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 239);

  look_result = tree.lookup(11);
  ASSERT_TRUE(look_result.has_value());
  EXPECT_EQ(*look_result.value(), 240);
}

TEST(BinarySearchTreeTest, DepthTest) {
  Ditto::BinarySearchTree<int, int> tree;
  // insert 1000 elements in ascending order
  for (uint32_t i = 0; i < 1000; i++) {
    tree.insert(i, i);
  }

  // The tree should be a linked list
  EXPECT_EQ(tree.depth(), 1000);

  for (uint32_t i = 0; i < 1000; i++) {
    auto result = tree.lookup(i);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result.value(), i);
  }
}

#include "ditto/red_black_tree.h"

#include <gtest/gtest.h>

#include <cmath>

TEST(RedBlackTreeTest, Constructor) { Ditto::RedBlackTree<int, int> tree; }

TEST(RedBlackTreeTest, InsertSingle) {
  Ditto::RedBlackTree<int, int> tree;
  tree.insert(12, 234);
}

TEST(RedBlackTreeTest, InsertMultiple) {
  Ditto::RedBlackTree<int, int> tree;
  tree.insert(12, 234);
  tree.insert(23, 235);
  tree.insert(25, 236);
  tree.insert(11, 237);
}

TEST(RedBlackTreeTest, Lookup) {
  Ditto::RedBlackTree<int, int> tree;
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

TEST(RedBlackTreeTest, DepthTest) {
  Ditto::RedBlackTree<int, int> tree;
  // insert 1000 elements in ascending order
  for (uint32_t i = 0; i < 1000; i++) {
    tree.insert(i, i);
  }

  // The depth must be at most 2 * log2(N + 1) for a red_black_tree
  EXPECT_LT(tree.depth(), 2 * std::log2(1000.0 + 1.0));

  // All values  should still be inside the tree
  for (uint32_t i = 0; i < 1000; i++) {
    auto result = tree.lookup(i);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result.value(), i);
  }

  auto result = tree.lookup(1000);
  ASSERT_FALSE(result.has_value());
}

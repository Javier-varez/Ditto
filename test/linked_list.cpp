
#include "ditto/linked_list.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <vector>

using Ditto::LinkedList;
using testing::ElementsAreArray;

TEST(LinkedList, PushBack) {
  LinkedList<const int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  EXPECT_EQ(list.size(), 3);
  ASSERT_THAT(list, ElementsAreArray(std::array{1, 3, 2}));
}

TEST(LinkedList, PushFront) {
  LinkedList<const int> list;
  list.push_front(1);
  list.push_front(3);
  list.push_front(2);

  EXPECT_EQ(list.size(), 3);
  ASSERT_THAT(list, ElementsAreArray(std::array{2, 3, 1}));
}

TEST(LinkedList, Iterator) {
  LinkedList<int> list;
  list.push_front(1);
  list.push_front(3);
  list.push_front(2);
  const LinkedList<int>& list_ref = list;

  std::array<int, 3> expected{2, 3, 1};
  int i = 0;
  for (auto iter = list_ref.begin(); iter != list_ref.end(); ++iter) {
    EXPECT_EQ(expected[i], *iter);
    i++;
  }
}

TEST(LinkedList, ReverseIterator) {
  LinkedList<int> list;
  list.push_front(1);
  list.push_front(3);
  list.push_front(2);
  const LinkedList<int>& list_ref = list;

  std::array<int, 3> expected{1, 3, 2};
  int i = 0;
  for (auto iter = list_ref.rbegin(); iter != list_ref.rend(); ++iter) {
    EXPECT_EQ(expected[i], *iter);
    i++;
  }
}

TEST(LinkedList, Insert) {
  LinkedList<const int> list;
  EXPECT_EQ(*list.insert(list.cbegin(), 1), 1);
  EXPECT_EQ(*list.insert(list.cbegin(), 3), 3);
  EXPECT_EQ(*list.insert(++list.cbegin(), 2), 2);
  EXPECT_EQ(*list.insert(list.cend(), 4), 4);

  EXPECT_EQ(list.size(), 4);
  ASSERT_THAT(list, ElementsAreArray(std::array{3, 2, 1, 4}));
}

TEST(LinkedList, Emplace) {
  LinkedList<const int> list;
  EXPECT_EQ(*list.emplace(list.cbegin(), 1), 1);
  EXPECT_EQ(*list.emplace(list.cbegin(), 3), 3);
  EXPECT_EQ(*list.emplace(++list.cbegin(), 2), 2);
  EXPECT_EQ(*list.emplace(list.cend(), 4), 4);

  EXPECT_EQ(list.size(), 4);
  ASSERT_THAT(list, ElementsAreArray(std::array{3, 2, 1, 4}));
}

TEST(LinkedList, PopBack) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  ASSERT_THAT(list, ElementsAreArray(std::array{1, 3, 2}));
  EXPECT_EQ(list.size(), 3);

  list.pop_back();

  ASSERT_THAT(list, ElementsAreArray(std::array{1, 3}));
  EXPECT_EQ(list.size(), 2);

  list.pop_back();

  ASSERT_THAT(list, ElementsAreArray(std::array{1}));
  EXPECT_EQ(list.size(), 1);

  list.pop_back();

  ASSERT_THAT(list, ElementsAreArray(std::vector<int>{}));
  EXPECT_EQ(list.size(), 0);
}

TEST(LinkedList, PopFront) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  ASSERT_THAT(list, ElementsAreArray(std::array{1, 3, 2}));
  EXPECT_EQ(list.size(), 3);

  list.pop_front();

  ASSERT_THAT(list, ElementsAreArray(std::array{3, 2}));
  EXPECT_EQ(list.size(), 2);

  list.pop_front();

  ASSERT_THAT(list, ElementsAreArray(std::array{2}));
  EXPECT_EQ(list.size(), 1);

  list.pop_front();

  ASSERT_THAT(list, ElementsAreArray(std::vector<int>{}));
  EXPECT_EQ(list.size(), 0);
}

TEST(LinkedList, Erase) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  ASSERT_THAT(list, ElementsAreArray(std::array{1, 3, 2}));
  EXPECT_EQ(list.size(), 3);

  EXPECT_EQ(*list.erase(++list.cbegin()), 2);

  ASSERT_THAT(list, ElementsAreArray(std::array{1, 2}));
  EXPECT_EQ(list.size(), 2);

  EXPECT_EQ(list.erase(++list.cbegin()), list.cend());

  ASSERT_THAT(list, ElementsAreArray(std::array{1}));
  EXPECT_EQ(list.size(), 1);

  EXPECT_EQ(list.erase(list.cbegin()), list.cend());

  ASSERT_THAT(list, ElementsAreArray(std::vector<int>{}));
  EXPECT_EQ(list.size(), 0);
}

TEST(LinkedList, Front) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  EXPECT_EQ(list.begin(), list.front_iter());
  EXPECT_EQ(1, *list.front_iter());
  EXPECT_EQ(1, list.front());
}

TEST(LinkedList, Back) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  EXPECT_EQ(2, *list.back_iter());
  EXPECT_EQ(2, list.back());
}

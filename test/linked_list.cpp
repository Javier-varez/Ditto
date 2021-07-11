
#include "linked_list.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::ElementsAreArray;

// TEST(LinkedList, IntList) {
//  LinkedList<int> list;
//  list.push_back(1);
//  list.push_back(3);
//  list.push_back(2);
//
//  list.walk([iter = 0U, expected = std::array{1, 3, 2}](int element) mutable {
//    if (iter < expected.size()) {
//      EXPECT_EQ(expected[iter], element);
//    } else {
//      FAIL();
//    }
//    iter++;
//  });
//
//  list.remove(list.tail());
//  list.remove(list.head());
//
//  list.walk([iter = 0](int value) mutable {
//    EXPECT_EQ(iter, 0);
//    EXPECT_EQ(value, 3);
//    iter++;
//  });
//}
//
// TEST(LinkedList, EmptyList) {
//  LinkedList<int> list;
//
//  EXPECT_EQ(list.head(), nullptr);
//  EXPECT_EQ(list.tail(), nullptr);
//
//  list.walk([](int /*unused*/) { FAIL(); });
//
//  // Remove should not crash and dereference memory
//  list.remove(nullptr);
//}
//
// TEST(LinkedList, RemoveInexistentElement) {
//  LinkedList<int> list;
//
//  list.push_back(0);
//
//  list.remove(reinterpret_cast<LinkedList<int>::Node*>(0xDEADC0DE));
//
//  EXPECT_NE(list.head(), nullptr);
//  EXPECT_EQ(list.head(), list.tail());
//}
//
// TEST(LinkedList, RemoveHead) {
//  LinkedList<int> list;
//  list.push_back(0);
//  list.push_back(1);
//
//  list.remove(list.head());
//
//  list.walk([iter = 0](int value) mutable {
//    EXPECT_EQ(iter, 0);
//    EXPECT_EQ(value, 1);
//    iter++;
//  });
//}
//
// TEST(LinkedList, RemoveTail) {
//  LinkedList<int> list;
//  list.push_back(0);
//  list.push_back(1);
//
//  list.remove(list.tail());
//
//  list.walk([iter = 0](int value) mutable {
//    EXPECT_EQ(iter, 0);
//    EXPECT_EQ(value, 0);
//    iter++;
//  });
//}
//
// TEST(LinkedList, FindElement) {
//  LinkedList<int> list;
//  list.push_back(3);
//  list.push_back(0);
//  list.push_back(2);
//  list.push_back(1);
//
//  const auto* element = list.find([](const int& value) { return value == 2;
//  });
//
//  EXPECT_EQ(element->get_element(), 2);
//  EXPECT_EQ(element->get_next(), list.tail());
//
//  element = list.find([](const int& /*unused*/) { return false; });
//  EXPECT_EQ(element, nullptr);
//}
//
// TEST(LinkedList, InsertAfter) {
//  LinkedList<int> list;
//  list.push_back(3);
//  list.push_back(0);
//  list.push_back(2);
//  list.push_back(1);
//
//  auto* element = list.head()->get_next()->get_next();
//  element->insert_after(123);
//
//  list.walk(
//      [iter = 0U, expected = std::array{3, 0, 2, 123, 1}](int element) mutable
//      {
//        if (iter < expected.size()) {
//          EXPECT_EQ(expected[iter], element);
//        } else {
//          FAIL();
//        }
//        iter++;
//      });
//}
//
// TEST(LinkedList, Emplace) {
//  LinkedList<std::string> list;
//  list.emplace_back("something");
//  list.emplace_front("something else");
//}
//
// TEST(LinkedList, EmplaceArgs) {
//  LinkedList<std::string> list;
//  list.emplace_back("something");
//  list.emplace_front("something else");
//  LinkedList<std::string>::Node* head = list.head();
//  head->emplace_after("Oh boy");
//
//  list.walk([iter = 0U, expected = std::array<std::string, 3>{
//                            "something else", "Oh boy",
//                            "something"}](const std::string& element) mutable
//                            {
//    if (iter < expected.size()) {
//      EXPECT_EQ(expected[iter], element);
//    } else {
//      FAIL();
//    }
//    iter++;
//  });
//}

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

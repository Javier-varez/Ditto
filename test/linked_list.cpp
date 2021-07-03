
#include "linked_list.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(LinkedList, IntList) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  list.walk([iter = 0U, expected = std::array{1, 3, 2}](int element) mutable {
    if (iter < expected.size()) {
      EXPECT_EQ(expected[iter], element);
    } else {
      FAIL();
    }
    iter++;
  });

  list.remove(list.tail());
  list.remove(list.head());

  list.walk([iter = 0](int value) mutable {
    EXPECT_EQ(iter, 0);
    EXPECT_EQ(value, 3);
    iter++;
  });
}

TEST(LinkedList, EmptyList) {
  LinkedList<int> list;

  EXPECT_EQ(list.head(), nullptr);
  EXPECT_EQ(list.tail(), nullptr);

  list.walk([](int /*unused*/) { FAIL(); });

  // Remove should not crash and dereference memory
  list.remove(nullptr);
}

TEST(LinkedList, RemoveInexistentElement) {
  LinkedList<int> list;

  list.push_back(0);

  list.remove(reinterpret_cast<LinkedList<int>::Node*>(0xDEADC0DE));

  EXPECT_NE(list.head(), nullptr);
  EXPECT_EQ(list.head(), list.tail());
}

TEST(LinkedList, RemoveHead) {
  LinkedList<int> list;
  list.push_back(0);
  list.push_back(1);

  list.remove(list.head());

  list.walk([iter = 0](int value) mutable {
    EXPECT_EQ(iter, 0);
    EXPECT_EQ(value, 1);
    iter++;
  });
}

TEST(LinkedList, RemoveTail) {
  LinkedList<int> list;
  list.push_back(0);
  list.push_back(1);

  list.remove(list.tail());

  list.walk([iter = 0](int value) mutable {
    EXPECT_EQ(iter, 0);
    EXPECT_EQ(value, 0);
    iter++;
  });
}

TEST(LinkedList, FindElement) {
  LinkedList<int> list;
  list.push_back(3);
  list.push_back(0);
  list.push_back(2);
  list.push_back(1);

  const auto* element = list.find([](const int& value) { return value == 2; });

  EXPECT_EQ(element->get_element(), 2);
  EXPECT_EQ(element->get_next(), list.tail());

  element = list.find([](const int& /*unused*/) { return false; });
  EXPECT_EQ(element, nullptr);
}

TEST(LinkedList, InsertAfter) {
  LinkedList<int> list;
  list.push_back(3);
  list.push_back(0);
  list.push_back(2);
  list.push_back(1);

  auto* element = list.head()->get_next()->get_next();
  element->insert_after(123);

  list.walk(
      [iter = 0U, expected = std::array{3, 0, 2, 123, 1}](int element) mutable {
        if (iter < expected.size()) {
          EXPECT_EQ(expected[iter], element);
        } else {
          FAIL();
        }
        iter++;
      });
}

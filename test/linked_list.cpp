
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "linked_list.h"

TEST(LinkedList, IntList) {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  list.walk([iter = 0](int element) mutable {
    switch (iter) {
      case 0:
        EXPECT_EQ(element, 1);
        break;
      case 1:
        EXPECT_EQ(element, 3);
        break;
      case 2:
        EXPECT_EQ(element, 2);
        break;
      default:
        FAIL();
        break;
    };
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

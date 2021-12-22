#include "ditto/enumerate.h"

#include <gtest/gtest.h>

TEST(EnumerateTest, ItWorks) {
  std::vector<int> data{1, 2, 3, 4, 5};

  size_t num_elements = 0;
  for (auto& [index, val_ptr] : make_enumerated_iter(data)) {
    EXPECT_EQ(index, *val_ptr - 1);
    num_elements++;
    *val_ptr = 0;
  }
  EXPECT_EQ(num_elements, data.size());

  num_elements = 0;
  for (const auto& [index, val_ptr] : make_enumerated_iter(data)) {
    EXPECT_EQ(*val_ptr, 0);
    num_elements++;
  }
  EXPECT_EQ(num_elements, data.size());
}

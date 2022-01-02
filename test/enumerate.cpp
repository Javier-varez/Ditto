#include "ditto/enumerate.h"

#include <gtest/gtest.h>

#include <map>

using Ditto::make_enumerated_iter;

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

TEST(EnumerateTest, EnumeratedMap) {
  std::map<std::string, int> data;
  data.insert({"string1", 1});
  data.insert({"string2", 2});
  data.insert({"string3", 3});

  size_t num_elements = 0;
  for (auto& [index, val_ptr] : make_enumerated_iter(data)) {
    const auto& str = val_ptr->first;
    if (str == "string1") {
      EXPECT_EQ(1, val_ptr->second);
      EXPECT_EQ(0, index);
    } else if (str == "string2") {
      EXPECT_EQ(2, val_ptr->second);
      EXPECT_EQ(1, index);
    } else if (str == "string3") {
      EXPECT_EQ(3, val_ptr->second);
      EXPECT_EQ(2, index);
    } else {
      FAIL();
    }
    val_ptr->second = 0;
    num_elements++;
  }
  EXPECT_EQ(num_elements, data.size());

  num_elements = 0;
  for (const auto& [index, val_ptr] : make_enumerated_iter(data)) {
    EXPECT_EQ(val_ptr->second, 0);
    num_elements++;
  }
  EXPECT_EQ(num_elements, data.size());
}

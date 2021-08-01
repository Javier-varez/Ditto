

#include "ditto/badge.h"

#include <gtest/gtest.h>

#include <memory>

class ClassWithBadge {
 public:
  void DoSomething(Ditto::Badge<class RegularClass> /*unused*/) {}
};

class RegularClass {
 public:
  void DoSomething(ClassWithBadge& obj) { obj.DoSomething({}); }
};

TEST(BadgeTest, test) {
  RegularClass regular;
  ClassWithBadge requires_badge;

  regular.DoSomething(requires_badge);
}

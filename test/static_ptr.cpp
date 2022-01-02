#include "ditto/static_ptr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using Ditto::static_ptr;
using testing::ElementsAreArray;

class Base {
 public:
  virtual ~Base() = default;
  virtual const char* type() = 0;
};

class Derived1 : public Base {
 public:
  explicit Derived1(int val) : m_val(val) {}

  const char* type() override { return "1"; }

  int get() { return m_val; }

 private:
  int m_val = 0;
};

class Derived2 : public Base {
 public:
  explicit Derived2(const char* val) : m_val(val) {}

  const char* type() override { return "2"; }

  const char* get() { return m_val; }

 private:
  const char* m_val = 0;
};

class Derived3 : public Base {
 public:
  explicit Derived3(const char* val) : m_val(val) {}

  const char* type() override { return "3"; }

  const char* get() { return m_val; }

 private:
  const char* m_val = 0;
};

TEST(StaticPtrTest, Constructor) {
  static_ptr<Base, Derived1, Derived2, Derived3> obj;
}

TEST(StaticPtrTest, Make) {
  static_ptr<Base, Derived1, Derived2, Derived3> obj;

  obj.make<Derived1>(1);
  EXPECT_STREQ(obj->type(), "1");
  EXPECT_NE(dynamic_cast<Derived1*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived2*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived3*>(obj.get()), nullptr);

  obj.make<Derived2>("test");
  EXPECT_STREQ(obj->type(), "2");
  EXPECT_NE(dynamic_cast<Derived2*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived1*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived3*>(obj.get()), nullptr);

  obj.make<Derived3>("test");
  EXPECT_STREQ(obj->type(), "3");
  EXPECT_NE(dynamic_cast<Derived3*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived1*>(obj.get()), nullptr);
  EXPECT_EQ(dynamic_cast<Derived2*>(obj.get()), nullptr);
}

TEST(StaticPtrTest, Reset) {
  static_ptr<Base, Derived3> obj;

  obj.make<Derived3>("");
  EXPECT_NE(obj.get(), nullptr);

  obj.reset();
  EXPECT_EQ(obj.get(), nullptr);
}

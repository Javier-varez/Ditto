
#include "ditto/circular_queue.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using Ditto::CircularQueue;
using testing::Mock;
using testing::StrictMock;

class Whiny {
 public:
  MOCK_METHOD(void, constructor, (), ());
  MOCK_METHOD(void, copy_constructor, (), ());
  MOCK_METHOD(void, move_constructor, (), ());
  MOCK_METHOD(void, copy_assignment_operator, (), ());
  MOCK_METHOD(void, move_assignment_operator, (), ());
  MOCK_METHOD(void, destructor, (), ());
};

class Element {
 public:
  Element(Whiny* whiny, int value) : m_whiny(whiny), m_value(value) {
    m_whiny->constructor();
  }

  Element(Element&& other) : m_whiny(other.m_whiny), m_value(other.m_value) {
    other.m_whiny = nullptr;
    other.m_value = 0;
    m_whiny->move_constructor();
  }

  Element(const Element& other)
      : m_whiny(other.m_whiny), m_value(other.m_value) {
    m_whiny->copy_constructor();
  }

  Element& operator=(Element&& other) {
    if (this != &other) {
      m_whiny = other.m_whiny;
      m_value = other.m_value;
      other.m_whiny = nullptr;
      other.m_value = 0;
      m_whiny->move_assignment_operator();
    }
    return *this;
  }

  Element& operator=(const Element& other) {
    if (this != &other) {
      m_whiny = other.m_whiny;
      m_value = other.m_value;
      m_whiny->copy_assignment_operator();
    }
    return *this;
  }

  ~Element() {
    if (m_whiny) {
      m_whiny->destructor();
    }
  }

  operator int() const { return m_value; }

 private:
  Whiny* m_whiny{nullptr};
  int m_value{0};
};

TEST(CircularQueueTest, EmplaceElement) {
  StrictMock<Whiny> whiny;
  CircularQueue<Element, 10> buffer;

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 23));

  ASSERT_NE(buffer.peek(), nullptr);
  EXPECT_EQ(*buffer.peek(), 23);

  EXPECT_CALL(whiny, destructor());
}

TEST(CircularQueueTest, PushElement) {
  StrictMock<Whiny> whiny;
  CircularQueue<Element, 10> buffer;

  EXPECT_CALL(whiny, constructor());
  EXPECT_CALL(whiny, move_constructor());
  EXPECT_TRUE(buffer.push({&whiny, 23}));

  ASSERT_NE(buffer.peek(), nullptr);
  EXPECT_EQ(*buffer.peek(), 23);

  EXPECT_CALL(whiny, destructor());
}

TEST(CircularQueueTest, PopElement) {
  StrictMock<Whiny> whiny;
  CircularQueue<Element, 10> buffer;

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 23));

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  {
    auto opt = buffer.pop();
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt.value(), 23);
  }

  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_FALSE(buffer.pop().has_value());
}

TEST(CircularQueueTest, PopMultipleElements) {
  StrictMock<Whiny> whiny;
  CircularQueue<Element, 10> buffer;

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 23));
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 32));
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  EXPECT_EQ(buffer.pop().value(), 23);
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  EXPECT_EQ(buffer.pop().value(), 32);
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_FALSE(buffer.pop().has_value());
}

TEST(CircularQueueTest, DoesntPushElementsWhenFull) {
  StrictMock<Whiny> whiny;
  CircularQueue<Element, 2> buffer;

  EXPECT_TRUE(buffer.empty());

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 23));
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 32));
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_TRUE(buffer.full());

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  EXPECT_EQ(buffer.pop().value(), 23);
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, constructor());
  EXPECT_TRUE(buffer.emplace(&whiny, 45));
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  EXPECT_EQ(buffer.pop().value(), 32);
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_CALL(whiny, move_constructor());
  EXPECT_CALL(whiny, destructor());
  EXPECT_EQ(buffer.pop().value(), 45);
  Mock::VerifyAndClearExpectations(&whiny);

  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.pop().has_value());
}

TEST(CircularQueueTest, DestructorDestroysElementsInTheBuffer) {
  constexpr std::size_t SIZE = 20;
  StrictMock<Whiny> whiny;
  CircularQueue<Element, SIZE> buffer;

  for (std::size_t i = 0; i < SIZE - 2; i++) {
    EXPECT_CALL(whiny, constructor());
    EXPECT_TRUE(buffer.emplace(&whiny, 23));
    Mock::VerifyAndClearExpectations(&whiny);
  }

  EXPECT_CALL(whiny, destructor()).Times(SIZE - 2);
}

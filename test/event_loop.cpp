
#include "ditto/event_loop.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>

using testing::InSequence;
using testing::StrictMock;

enum class Event {
  SOMETHING,
  SOMETHING_ELSE,
};

class Hal {
 public:
  MOCK_METHOD(void, wfe, (), ());
  MOCK_METHOD(void, disable_interrupts, (), ());
  MOCK_METHOD(void, enable_interrupts, (), ());
};

using EventLoop = Ditto::EventLoop<Event, Hal>;

class MockListener : public EventLoop::Listener {
 public:
  MockListener() = default;

  MOCK_METHOD(void, on_event, (Event), (override));
};

TEST(EventLoopTest, TestLoop) {
  StrictMock<MockListener> listener1;
  StrictMock<MockListener> listener2;
  StrictMock<MockListener> listener3;
  StrictMock<Hal> hal;
  EventLoop loop{&hal};

  loop.register_listener(Event::SOMETHING, &listener1);
  loop.register_listener(Event::SOMETHING_ELSE, &listener2);
  loop.register_listener(&listener3);

  InSequence s;
  EXPECT_CALL(hal, disable_interrupts());
  EXPECT_CALL(hal, enable_interrupts());
  EXPECT_CALL(hal, wfe()).WillOnce([&loop]() {
    loop.post_event(Event::SOMETHING);
  });
  EXPECT_CALL(hal, disable_interrupts());
  EXPECT_CALL(hal, enable_interrupts());
  EXPECT_CALL(listener1, on_event(Event::SOMETHING));
  EXPECT_CALL(listener3, on_event(Event::SOMETHING));

  EXPECT_CALL(hal, disable_interrupts());
  EXPECT_CALL(hal, enable_interrupts());
  EXPECT_CALL(hal, wfe()).WillOnce([&loop]() {
    loop.post_event(Event::SOMETHING_ELSE);
  });
  EXPECT_CALL(hal, disable_interrupts());
  EXPECT_CALL(hal, enable_interrupts());
  EXPECT_CALL(listener2, on_event(Event::SOMETHING_ELSE));
  EXPECT_CALL(listener3, on_event(Event::SOMETHING_ELSE)).WillOnce([&loop]() {
    loop.stop();
  });

  std::thread t{[&loop]() { loop.run(); }};
  t.join();
}

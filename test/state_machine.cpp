
#include "ditto/state_machine.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using Ditto::State;
using Ditto::StateMachine;
using testing::InSequence;
using testing::StrictMock;

enum class GarageDoorStateId { CLOSING, CLOSED, OPEN, OPENING };
enum class GarageDoorEventId { FULLY_CLOSED, FULLY_OPEN, BUTTON_PRESSED };

using GarageDoorState = State<GarageDoorStateId, GarageDoorEventId>;

class StateMock {
 public:
  MOCK_METHOD(void, constructed, (GarageDoorStateId), ());
  MOCK_METHOD(void, on_entry, (GarageDoorStateId), ());
  MOCK_METHOD(void, handle_event, (GarageDoorStateId, GarageDoorEventId), ());
  MOCK_METHOD(void, on_exit, (GarageDoorStateId), ());
  MOCK_METHOD(void, destroyed, (GarageDoorStateId), ());
};

template <GarageDoorStateId STATE_ID>
class GarageDoorStateFake : public GarageDoorState {
 public:
  GarageDoorStateFake(StateMock* mock) : m_mock(mock) {
    m_mock->constructed(STATE_ID);
  }
  void on_entry() { m_mock->on_entry(STATE_ID); }
  void handle_event(GarageDoorEventId e) { m_mock->handle_event(STATE_ID, e); }
  void on_exit() { m_mock->on_exit(STATE_ID); }
  ~GarageDoorStateFake() { m_mock->destroyed(STATE_ID); }

 private:
  StateMock* m_mock;
};

using GarageDoorStateOpen = GarageDoorStateFake<GarageDoorStateId::OPEN>;
using GarageDoorStateClosed = GarageDoorStateFake<GarageDoorStateId::CLOSED>;
using GarageDoorStateClosing = GarageDoorStateFake<GarageDoorStateId::CLOSING>;
using GarageDoorStateOpening = GarageDoorStateFake<GarageDoorStateId::OPENING>;

using GarageDoorSM =
    StateMachine<GarageDoorStateId, GarageDoorEventId, GarageDoorStateOpen,
                 GarageDoorStateClosed, GarageDoorStateClosing,
                 GarageDoorStateOpening>;

using StateTransition = GarageDoorSM::StateTransition;

std::array transition_table{
    StateTransition{GarageDoorStateId::OPEN, GarageDoorStateId::CLOSING,
                    GarageDoorEventId::BUTTON_PRESSED},
    StateTransition{GarageDoorStateId::CLOSING, GarageDoorStateId::OPENING,
                    GarageDoorEventId::BUTTON_PRESSED},
    StateTransition{GarageDoorStateId::CLOSING, GarageDoorStateId::CLOSED,
                    GarageDoorEventId::FULLY_CLOSED},
    StateTransition{GarageDoorStateId::CLOSED, GarageDoorStateId::OPENING,
                    GarageDoorEventId::BUTTON_PRESSED},
    StateTransition{GarageDoorStateId::OPENING, GarageDoorStateId::CLOSING,
                    GarageDoorEventId::BUTTON_PRESSED},
    StateTransition{GarageDoorStateId::OPENING, GarageDoorStateId::OPEN,
                    GarageDoorEventId::FULLY_OPEN}};

class GarageDoorStateFactory : public GarageDoorSM::StateFactory {
 public:
  GarageDoorStateFactory(StateMock* mock) : m_mock(mock) {}
  void make_state(GarageDoorStateId id,
                  GarageDoorSM::PolymorphicState* state) const override {
    switch (id) {
      case GarageDoorStateId::OPEN:
        state->make<GarageDoorStateOpen>(m_mock);
        break;
      case GarageDoorStateId::CLOSED:
        state->make<GarageDoorStateClosed>(m_mock);
        break;
      case GarageDoorStateId::OPENING:
        state->make<GarageDoorStateOpening>(m_mock);
        break;
      case GarageDoorStateId::CLOSING:
        state->make<GarageDoorStateClosing>(m_mock);
        break;
    }
  }

 private:
  StateMock* m_mock;
};

TEST(StateMachineTest, Constructor) {
  StrictMock<StateMock> mock;
  GarageDoorStateFactory factory{&mock};

  InSequence s;

  EXPECT_CALL(mock, constructed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::CLOSING));

  GarageDoorSM sm{transition_table, &factory};

  EXPECT_CALL(mock, destroyed(GarageDoorStateId::CLOSING));
}

TEST(StateMachineTest, TransitionToAnotherState) {
  StrictMock<StateMock> mock;
  GarageDoorStateFactory factory{&mock};

  InSequence s;

  EXPECT_CALL(mock, constructed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::CLOSING));

  GarageDoorSM sm{transition_table, &factory};

  EXPECT_CALL(mock, handle_event(GarageDoorStateId::CLOSING,
                                 GarageDoorEventId::BUTTON_PRESSED));
  EXPECT_CALL(mock, on_exit(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, destroyed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, constructed(GarageDoorStateId::OPENING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::OPENING));

  sm.handle_event(GarageDoorEventId::BUTTON_PRESSED);

  EXPECT_CALL(mock, destroyed(GarageDoorStateId::OPENING));
}

TEST(StateMachineTest, EventWithoutTransition) {
  StrictMock<StateMock> mock;
  GarageDoorStateFactory factory{&mock};

  InSequence s;

  EXPECT_CALL(mock, constructed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::CLOSING));

  GarageDoorSM sm{transition_table, &factory};

  EXPECT_CALL(mock, handle_event(GarageDoorStateId::CLOSING,
                                 GarageDoorEventId::FULLY_OPEN));

  sm.handle_event(GarageDoorEventId::FULLY_OPEN);

  EXPECT_CALL(mock, destroyed(GarageDoorStateId::CLOSING));
}

TEST(StateMachineTest, MultipleTransitions) {
  StrictMock<StateMock> mock;
  GarageDoorStateFactory factory{&mock};

  InSequence s;

  EXPECT_CALL(mock, constructed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::CLOSING));

  GarageDoorSM sm{transition_table, &factory};

  EXPECT_CALL(mock, handle_event(GarageDoorStateId::CLOSING,
                                 GarageDoorEventId::BUTTON_PRESSED));
  EXPECT_CALL(mock, on_exit(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, destroyed(GarageDoorStateId::CLOSING));
  EXPECT_CALL(mock, constructed(GarageDoorStateId::OPENING));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::OPENING));

  sm.handle_event(GarageDoorEventId::BUTTON_PRESSED);

  EXPECT_CALL(mock, handle_event(GarageDoorStateId::OPENING,
                                 GarageDoorEventId::FULLY_OPEN));
  EXPECT_CALL(mock, on_exit(GarageDoorStateId::OPENING));
  EXPECT_CALL(mock, destroyed(GarageDoorStateId::OPENING));
  EXPECT_CALL(mock, constructed(GarageDoorStateId::OPEN));
  EXPECT_CALL(mock, on_entry(GarageDoorStateId::OPEN));

  sm.handle_event(GarageDoorEventId::FULLY_OPEN);

  EXPECT_CALL(mock, destroyed(GarageDoorStateId::OPEN));
}

#ifndef DITTO_STATE_MACHINE_H_
#define DITTO_STATE_MACHINE_H_

#include "ditto/span.h"
#include "ditto/static_ptr.h"

namespace Ditto {

template <class StateId, class EventId>
class State {
 public:
  virtual void on_entry() {}
  virtual void on_exit() {}
  virtual void handle_event(EventId) {}

  virtual ~State() = default;
};

template <typename StateId, typename EventId, class... States>
class StateMachine {
 public:
  struct StateTransition {
    StateId current_state_id;
    StateId next_state_id;
    EventId event;
  };

  using PolymorphicState =
      Ditto::static_ptr<State<StateId, EventId>, States...>;

  class StateFactory {
   public:
    virtual void make_state(StateId id, PolymorphicState* state) const = 0;
    virtual void make_state(const StateTransition& transition,
                            PolymorphicState* state) const {
      make_state(transition.next_state_id, state);
    }
  };

  StateMachine(Ditto::span<const StateTransition> state_transition_table,
               const StateFactory* state_factory)
      : m_state_transition_table(state_transition_table),
        m_state_factory(*state_factory) {
    // Make the reset state
    m_state_factory.make_state(m_state_id, &m_state);
    m_state->on_entry();
  }

  void handle_event(EventId e) {
    m_state->handle_event(e);
    auto* transition = lookup_transition(e);
    if (transition) {
      m_state->on_exit();
      m_state_factory.make_state(*transition, &m_state);
      m_state_id = transition->next_state_id;
      m_state->on_entry();
    }
  }

 private:
  Ditto::span<const StateTransition> m_state_transition_table;
  const StateFactory& m_state_factory;
  StateId m_state_id = static_cast<StateId>(0);
  PolymorphicState m_state;

  const StateTransition* lookup_transition(EventId event) {
    for (const auto& transition : m_state_transition_table) {
      if ((transition.event == event) &&
          (transition.current_state_id == m_state_id)) {
        return &transition;
      }
    }
    return nullptr;
  }
};

}  // namespace Ditto

#endif  // DITTO_STATE_MACHINE_H_

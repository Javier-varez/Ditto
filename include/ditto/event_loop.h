#ifndef DITTO_EVENT_LOOP_H_
#define DITTO_EVENT_LOOP_H_

#include <atomic>
#include <cstddef>
#include <optional>

#include "ditto/circular_queue.h"
#include "ditto/linear_map.h"
#include "ditto/non_null_ptr.h"

namespace Ditto {

template <class Event, class HAL, std::size_t MAX_INFLIGHT_EVENTS = 10,
          std::size_t MAX_LISTENERS = 10>
class EventLoop {
 public:
  class Listener {
   public:
    virtual void on_event(Event event) = 0;

    virtual ~Listener() = default;
  };

  explicit EventLoop(HAL* hal) : m_hal(hal) {}

  void post_event(Event e) { m_event_queue.push(e); }

  void run() {
    while (m_running.load(std::memory_order_relaxed)) {
      m_hal->disable_interrupts();
      std::optional<Event> event = m_event_queue.pop();
      if (!event.has_value()) {
        m_hal->enable_interrupts();
        m_hal->wfe();
      } else {
        // Look for subscribers and notify them
        std::optional<NonNullPtr<Listener>> listener =
            m_listeners.at(event.value());
        Listener* broadcast = m_broadcast;
        m_hal->enable_interrupts();

        if (listener.has_value()) {
          listener.value()->on_event(event.value());
        }
        if (broadcast) {
          broadcast->on_event(event.value());
        }
      }
    }
  }

  void register_listener(Event event, NonNullPtr<Listener> listener) {
    m_listeners[event] = listener;
  }

  void register_listener(NonNullPtr<Listener> listener) {
    m_broadcast = listener.get();
  }

  void stop() { m_running.store(false, std::memory_order_relaxed); }

 private:
  HAL* m_hal = nullptr;
  CircularQueue<Event, MAX_INFLIGHT_EVENTS> m_event_queue;
  LinearMap<Event, NonNullPtr<Listener>, MAX_LISTENERS> m_listeners;
  Listener* m_broadcast = nullptr;

  std::atomic_bool m_running{true};
};

}  // namespace Ditto

#endif  // DITTO_EVENT_LOOP_H_

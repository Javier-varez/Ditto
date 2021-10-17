
#pragma once

#include <mutex>
#include <utility>

namespace Ditto {

template <class T, class M>
class ResourceLock {
 public:
  template <class... Args>
  constexpr explicit ResourceLock(Args... args)
      : m_resource(std::forward<Args>(args)...) {}

  template <class Action,
            std::enable_if_t<std::is_invocable_v<Action, T&>, bool> = false>
  inline void lock(Action action) {
    std::scoped_lock<M> lock{m_mutex};
    action(m_resource);
  }

  // TODO(javier-varez): Handle copy and move constructors and assignment
  // operators to also lock the object when reading from it. For now these two
  // operations are disallowed
  ResourceLock(const ResourceLock&) = delete;
  ResourceLock(ResourceLock&&) = delete;
  auto operator=(const ResourceLock&) -> ResourceLock& = delete;
  auto operator=(ResourceLock &&) -> ResourceLock& = delete;

 private:
  M m_mutex;
  T m_resource;
};

}  // namespace Ditto

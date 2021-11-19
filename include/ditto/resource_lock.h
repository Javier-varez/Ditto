#ifndef DITTO_RESOURCE_LOCK_H_
#define DITTO_RESOURCE_LOCK_H_

#include <stdint.h>

#include <mutex>
#include <type_traits>
#include <utility>

#include "ditto/type_traits.h"

namespace Ditto {

template <class T, class M>
class ResourceLock {
 public:
  template <class... Args>
  constexpr explicit ResourceLock(Args... args)
      : m_resource(std::forward<Args>(args)...) {}

  template <class Action,
            std::enable_if_t<std::is_invocable_v<Action, T&>, bool> = false>
  inline auto lock(Action action) {
    std::scoped_lock<M> lock{m_mutex};
    return action(m_resource);
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

template <class T, class M>
class ReadWriteLock {
 public:
  template <class... Args>
  constexpr explicit ReadWriteLock(Args... args)
      : m_resource(std::forward<Args>(args)...) {}

  template <class Action,
            std::enable_if_t<std::is_invocable_v<Action, T&>, bool> = false>
  inline auto write_lock(Action action) {
    std::scoped_lock<M> lock{m_global_mutex};
    return action(m_resource);
  }

  template <class Action,
            std::enable_if_t<std::is_invocable_v<Action, const T&> &&
                                 !Ditto::returns_void_v<Action, const T&>,
                             bool> = false>
  inline auto read_lock(Action action) {
    {
      std::scoped_lock<M> r_lock{m_read_mutex};
      m_num_readers++;
      if (m_num_readers == 1) {
        m_global_mutex.lock();
      }
    }

    auto retval = action(static_cast<const T>(m_resource));

    {
      std::scoped_lock<M> r_lock{m_read_mutex};
      m_num_readers--;
      if (m_num_readers == 0) {
        m_global_mutex.unlock();
      }
    }

    return retval;
  }

  template <class Action,
            std::enable_if_t<std::is_invocable_v<Action, const T&> &&
                                 Ditto::returns_void_v<Action, const T&>,
                             bool> = false>
  inline void read_lock(Action action) {
    {
      std::scoped_lock<M> r_lock{m_read_mutex};
      m_num_readers++;
      if (m_num_readers == 1) {
        m_global_mutex.lock();
      }
    }

    action(static_cast<const T>(m_resource));

    {
      std::scoped_lock<M> r_lock{m_read_mutex};
      m_num_readers--;
      if (m_num_readers == 0) {
        m_global_mutex.unlock();
      }
    }
  }

  // TODO(javier-varez): Handle copy and move constructors and assignment
  // operators to also lock the object when reading from it. For now these two
  // operations are disallowed
  ReadWriteLock(const ReadWriteLock&) = delete;
  ReadWriteLock(ReadWriteLock&&) = delete;
  auto operator=(const ReadWriteLock&) -> ReadWriteLock& = delete;
  auto operator=(ReadWriteLock &&) -> ReadWriteLock& = delete;

 private:
  std::uint32_t m_num_readers = 0;
  M m_read_mutex;
  M m_global_mutex;
  T m_resource;
};

}  // namespace Ditto

#endif  // DITTO_RESOURCE_LOCK_H_

#ifndef DITTO_NON_NULL_PTR_H_
#define DITTO_NON_NULL_PTR_H_

#include <memory>

#include "ditto/assert.h"

namespace Ditto {
/**
 * @brief Non owning pointer that cannot be null
 */

template <class T>
class NonNullPtr final {
 public:
  NonNullPtr(T* _Nonnull ptr) : m_ptr(ptr) { DITTO_VERIFY(m_ptr != nullptr); }

  NonNullPtr(const std::unique_ptr<T>& ptr) : m_ptr(ptr.get()) {
    DITTO_VERIFY(m_ptr != nullptr);
  }

  NonNullPtr(const std::shared_ptr<T>& ptr) : m_ptr(ptr.get()) {
    DITTO_VERIFY(m_ptr != nullptr);
  }

  NonNullPtr(const NonNullPtr&) = default;
  NonNullPtr(NonNullPtr&&) = default;
  NonNullPtr& operator=(const NonNullPtr&) = default;
  NonNullPtr& operator=(NonNullPtr&&) = default;

  [[nodiscard]] T* _Nonnull get() const { return m_ptr; }

  [[nodiscard]] T* _Nonnull operator->() const { return m_ptr; }

  [[nodiscard]] T& operator*() const { return *m_ptr; }

 private:
  T* _Nonnull m_ptr;
};

}  // namespace Ditto

#endif  // DITTO_NON_NULL_PTR_H_

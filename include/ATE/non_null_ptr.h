
#pragma once

#include "ATE/assert.h"

namespace ATE {
/**
 * @brief Non owning pointer that cannot be null
 */

template <class T>
class NonNullPtr final {
 public:
  NonNullPtr(T* _Nonnull ptr) : m_ptr(ptr) { ATE_VERIFY(m_ptr != nullptr); }

  NonNullPtr(const NonNullPtr&) = default;
  NonNullPtr(NonNullPtr&&) = default;
  NonNullPtr& operator=(const NonNullPtr&) = default;
  NonNullPtr& operator=(NonNullPtr&&) = default;

  T* _Nonnull get() const { return m_ptr; }

  T* _Nonnull operator->() const { return m_ptr; }

  T& operator*() const { return *m_ptr; }

 private:
  T* _Nonnull m_ptr;
};

}  // namespace ATE

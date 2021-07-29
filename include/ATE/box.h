
#pragma once

#include <utility>

namespace ATE {
template <class T>
class Box {
 public:
  Box() noexcept = default;
  explicit Box(T* pointer) noexcept : m_contents(pointer) {}

  Box(Box&& other) noexcept { *this = std::move(other); }

  auto operator=(Box&& other) noexcept -> Box& {
    if (this != &other) {
      reset();
      m_contents = other.m_contents;
      other.m_contents = nullptr;
    }
    return *this;
  }

  Box(const Box& box) = delete;
  auto operator=(const Box& box) -> Box& = delete;

  ~Box() { reset(); }

  explicit operator bool() const noexcept { return m_contents != nullptr; }

  // Operators that access the internal pointer should not be called on a
  // temporary, as the pointer is immediately released.
  //
  // Therefore, these operators and methods are declared as methods for lvalue
  // references.
  [[nodiscard]] auto operator->() const& noexcept -> T* { return m_contents; }
  [[nodiscard]] auto operator*() const& noexcept -> T& { return *m_contents; }
  [[nodiscard]] auto get() const& noexcept -> T* { return m_contents; }

  // Calling reset on a temporary does not make sense, it will be destroyed
  // anyways.
  auto reset() noexcept {
    if (m_contents != nullptr) {
      delete m_contents;
      m_contents = nullptr;
    }
  }

 private:
  T* m_contents = nullptr;
};

template <class T, class... Args>
inline auto make_box(Args... args) -> Box<T> {
  return Box<T>{new T{std::forward<Args>(args)...}};
}

}  // namespace ATE

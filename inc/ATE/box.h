
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

  [[nodiscard]] auto operator->() const noexcept -> T* { return m_contents; }

  [[nodiscard]] auto operator*() const noexcept -> T& { return *m_contents; }

  [[nodiscard]] auto get() const noexcept -> T* { return m_contents; }

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

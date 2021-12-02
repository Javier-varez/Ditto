#ifndef DITTO_BOX_H_
#define DITTO_BOX_H_

#include <utility>

namespace Ditto {
template <class T>
class Box {
 public:
  template <class... Args>
  explicit Box(Args... args) noexcept {
    m_contents = new T{std::forward<Args>(args)...};
  }

  // Moving requires being able to default construct a T
  Box(Box&& other) noexcept {
    m_contents = other.m_contents;
    other.m_contents = new T{};
  }

  // Moving requires being able to default construct a T
  auto operator=(Box&& other) noexcept -> Box& {
    if (this != &other) {
      delete m_contents;
      m_contents = other.m_contents;
      other.m_contents = new T{};
    }
    return *this;
  }

  Box(const Box& box) = delete;
  auto operator=(const Box& box) -> Box& = delete;

  ~Box() {
    if (m_contents != nullptr) {
      delete m_contents;
      m_contents = nullptr;
    }
  }

  // Operators that access the internal pointer should not be called on a
  // temporary, as the pointer is immediately released.
  //
  // Therefore, these operators and methods are declared as methods for lvalue
  // references.
  [[nodiscard]] auto operator->() const& noexcept -> T* { return m_contents; }
  [[nodiscard]] auto operator*() const& noexcept -> T& { return *m_contents; }
  [[nodiscard]] auto get() const& noexcept -> T* { return m_contents; }

  // Sometimes we may want to leak memory intentionally. In this case we could
  // call this function to release ownership of the Box and return a bare
  // pointer to it. Afterwards, the Box is default initialized with a new T.
  [[nodiscard]] auto leak() -> T* {
    auto leaked_ptr = m_contents;
    m_contents = new T{};
    return leaked_ptr;
  }

 private:
  T* m_contents = nullptr;
};

}  // namespace Ditto

#endif  // DITTO_BOX_H_

#ifndef DITTO_PAIR_H_
#define DITTO_PAIR_H_

#include <utility>

namespace Ditto {
template <class L, class R>
class Pair {
 public:
  Pair(const L& l, const R& r) : m_left(l), m_right(r) {}

  // forwarding constructor for the right element
  // Left element is still copied
  template <class... T>
  explicit Pair(const L& l, T... t)
      : m_left(l), m_right(std::forward<T>(t)...) {}

  [[nodiscard]] auto left() const -> const L& { return m_left; }

  [[nodiscard]] auto left() -> L& { return m_left; }

  [[nodiscard]] auto right() const -> const R& { return m_right; }

  [[nodiscard]] auto right() -> R& { return m_right; }

 private:
  L m_left;
  R m_right;
};

}  // namespace Ditto

#endif  // DITTO_PAIR_H_

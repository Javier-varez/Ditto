
#pragma once

namespace ATE {
template <class L, class R>
class Pair {
 public:
  Pair(const L& l, const R& r) : m_left(l), m_right(r) {}

  auto left() const -> const L& { return m_left; }

  auto left() -> L& { return m_left; }

  auto right() const -> const R& { return m_right; }

  auto right() -> R& { return m_right; }

 private:
  L m_left;
  R m_right;
};

}  // namespace ATE

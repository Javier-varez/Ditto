#ifndef DITTO_ENUMERATE_H_
#define DITTO_ENUMERATE_H_

#include <iterator>

namespace Ditto {

template <class inner_iter>
class EnumIterator {
 public:
  using inner_value_type =
      typename std::iterator_traits<inner_iter>::value_type;
  using inner_reference = typename std::iterator_traits<inner_iter>::reference;
  using inner_pointer = typename std::iterator_traits<inner_iter>::pointer;

  using difference_type =
      typename std::iterator_traits<inner_iter>::difference_type;
  using value_type = std::tuple<size_t, inner_pointer>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename std::iterator_traits<inner_iter>::pointer;
  using iterator_category = std::bidirectional_iterator_tag;

  EnumIterator(inner_iter inner, size_t index)
      : m_inner(inner), m_index(index), m_tuple({m_index, &*m_inner}) {}

  auto operator*() -> reference {
    m_tuple = {m_index, &*m_inner};
    return m_tuple;
  }

  auto operator++() -> EnumIterator& {
    m_inner++;
    m_index++;
    return *this;
  }

  auto operator--() -> EnumIterator& {
    m_inner--;
    m_index--;
    return *this;
  }

  auto operator==(EnumIterator other) const -> bool {
    return m_inner == other.m_inner;
  }

  auto operator!=(EnumIterator other) const -> bool {
    return m_inner != other.m_inner;
  }

  size_t index() const { return m_index; }

 private:
  inner_iter m_inner;
  size_t m_index;

  std::tuple<size_t, inner_pointer> m_tuple;
};

template <class Container>
class EnumContainer {
 public:
  explicit EnumContainer(Container& container) : m_inner(container) {}

  auto begin() -> EnumIterator<typename Container::iterator> {
    return EnumIterator(m_inner.begin(), 0);
  }

  auto end() -> EnumIterator<typename Container::iterator> {
    return EnumIterator(m_inner.end(), m_inner.size());
  }

  auto cbegin() const -> EnumIterator<typename Container::const_iterator> {
    return EnumIterator(m_inner.cbegin(), 0);
  }

  auto cend() const -> EnumIterator<typename Container::const_iterator> {
    return EnumIterator(m_inner.cend(), m_inner.size());
  }

 private:
  Container& m_inner;

  static_assert(
      !std::is_same_v<typename std::iterator_traits<
                          typename Container::iterator>::iterator_category,
                      std::input_iterator_tag>,
      "Iterator must be at least bidirectional");
  static_assert(
      !std::is_same_v<typename std::iterator_traits<
                          typename Container::iterator>::iterator_category,
                      std::forward_iterator_tag>,
      "Iterator must be at least bidirectional");
};

template <class Container>
auto make_enumerated_iter(Container& container) -> EnumContainer<Container> {
  return EnumContainer{container};
}

}  // namespace Ditto

#endif  // DITTO_ENUMERATE_H_

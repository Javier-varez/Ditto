
#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

template <class T>
class LinkedList;

template <class T>
struct LinkedListNode {
  T m_element;
  std::unique_ptr<LinkedListNode> m_next = nullptr;
  LinkedListNode* m_prev = nullptr;

 public:
  explicit LinkedListNode(const T& element) : m_element(element) {}
  explicit LinkedListNode(T&& element) : m_element(std::move(element)) {}

  template <class... Args>
  explicit LinkedListNode(Args&&... args)
      : m_element(std::forward<Args>(args)...) {}
};

template <class T, bool REVERSE = false>
class LinkedListIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::bidirectional_iterator_tag;

  explicit LinkedListIterator(LinkedListNode<std::remove_const_t<T>>* node)
      : m_current(node) {}

  // Post-increment and post-decrement
  auto operator++(int) -> LinkedListIterator {
    LinkedListIterator current = *this;
    if constexpr (REVERSE) {
      move_to_prev();
    } else {
      move_to_next();
    }
    return current;
  }

  auto operator--(int) -> LinkedListIterator {
    LinkedListIterator current = *this;
    if constexpr (REVERSE) {
      move_to_next();
    } else {
      move_to_prev();
    }
    return current;
  }

  // Pre-increment and pre-decrement
  auto operator++() -> LinkedListIterator& {
    if constexpr (REVERSE) {
      move_to_prev();
    } else {
      move_to_next();
    }
    return *this;
  }

  auto operator--() -> LinkedListIterator& {
    if constexpr (REVERSE) {
      move_to_next();
    } else {
      move_to_prev();
    }
    return *this;
  }

  auto operator*() -> T& { return m_current->m_element; }
  auto operator*() const -> const T& { return m_current->m_element; }

  auto operator->() -> T* { return &m_current->m_element; }
  auto operator->() const -> const T* { return &m_current->m_element; }

  template <class U, bool REV>
  auto operator==(LinkedListIterator<U, REV> other) const -> bool {
    return m_current == other.m_current;
  }

  template <class U, bool REV>
  auto operator!=(LinkedListIterator<U, REV> other) const -> bool {
    return m_current != other.m_current;
  }

  template <bool REV>
  operator LinkedListIterator<const T, REV>() const {
    return LinkedListIterator<const T, REV>{m_current};
  }

 private:
  LinkedListNode<std::remove_const_t<T>>* m_current;

  auto move_to_next() {
    if (m_current) {
      m_current = m_current->m_next.get();
    }
  }

  auto move_to_prev() {
    if (m_current) {
      m_current = m_current->m_prev;
    }
  }

  template <class U>
  friend class LinkedList;

  template <class U, bool REV>
  friend class LinkedListIterator;
};

template <class T>
class LinkedList {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = value_type&;
  using iterator = LinkedListIterator<value_type>;
  using const_iterator = LinkedListIterator<const value_type>;
  using reverse_iterator = LinkedListIterator<value_type, true>;
  using const_reverse_iterator = LinkedListIterator<const value_type, true>;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  // Node elements should not be constant so that they can always be used by
  // iterators and converted
  using Node = LinkedListNode<std::remove_const_t<T>>;

  LinkedList() = default;

  auto begin() -> iterator { return iterator{m_head.get()}; }
  auto end() -> iterator { return iterator{nullptr}; }
  auto begin() const -> const_iterator { return const_iterator{m_head.get()}; }
  auto end() const -> const_iterator { return const_iterator{nullptr}; }
  auto cbegin() const -> const_iterator { return const_iterator{m_head.get()}; }
  auto cend() const -> const_iterator { return const_iterator{nullptr}; }

  auto rbegin() -> reverse_iterator { return reverse_iterator{m_tail}; }
  auto rend() -> reverse_iterator { return reverse_iterator{nullptr}; }
  auto rbegin() const -> const_reverse_iterator {
    return const_reverse_iterator{m_tail};
  }
  auto rend() const -> const_reverse_iterator {
    return const_reverse_iterator{nullptr};
  }
  auto crbegin() const -> const_reverse_iterator {
    return const_reverse_iterator{m_tail};
  }
  auto crend() const -> const_reverse_iterator {
    return const_reverse_iterator{nullptr};
  }

  auto front() -> reference { return m_head->m_element; }
  auto front() const -> const_reference { return m_head->m_element; }
  auto back() -> reference { return m_tail->m_element; }
  auto back() const -> const_reference { return m_tail->m_element; }

  auto front_iter() -> iterator { return iterator{m_head.get()}; }
  auto front_iter() const -> const_iterator {
    return const_iterator{m_head.get()};
  }
  auto back_iter() -> iterator { return iterator{m_tail}; }
  auto back_iter() const -> const_iterator { return const_iterator{m_tail}; }

  [[nodiscard]] auto empty() const -> bool { return m_head == nullptr; }
  [[nodiscard]] auto size() const -> size_type {
    size_type count = 0;
    for (const auto& val : *this) {
      count++;
    }
    return count;
  }

  auto clear() {
    m_head = nullptr;
    m_tail = nullptr;
  }

  // Inserts an element before the passed iterator and returns an iterator to it
  iterator insert(const_iterator iter, const T& value) {
    Node* current = iter.m_current;
    if (current) {
      if (Node* prev = current->m_prev) {
        auto new_node = std::make_unique<Node>(value);
        new_node->m_next = std::move(prev->m_next);
        new_node->m_prev = prev;
        if (new_node->m_next) {
          current->m_prev = new_node.get();
        }
        prev->m_next = std::move(new_node);
        return iterator{prev->m_next.get()};
      } else {
        push_front(value);
        return begin();
      }
    } else {
      // We are at the beginning of the list already
      push_back(value);
      return iterator{m_tail};
    }
  }

  // Inserts an element before the passed iterator and returns an iterator to it
  template <class... Args>
  iterator emplace(const_iterator iter, Args&&... args) {
    Node* current = iter.m_current;
    if (current) {
      if (Node* prev = current->m_prev) {
        auto new_node = std::make_unique<Node>(std::forward<Args>(args)...);
        new_node->m_next = std::move(prev->m_next);
        new_node->m_prev = prev;
        if (new_node->m_next) {
          current->m_prev = new_node.get();
        }
        prev->m_next = std::move(new_node);
        return iterator{prev->m_next.get()};
      } else {
        emplace_front(std::forward<Args>(args)...);
        return begin();
      }
    } else {
      // We are at the beginning of the list already
      emplace_back(std::forward<Args>(args)...);
      return iterator{m_tail};
    }
  }

  void push_back(T element) {
    auto node = std::make_unique<Node>(std::move(element));
    node->m_prev = m_tail;
    if (m_tail) {
      m_tail->m_next = std::move(node);
      m_tail = m_tail->m_next.get();
    } else {
      m_head = std::move(node);
      m_tail = m_head.get();
    }
  }

  void push_front(T element) {
    auto new_head = std::make_unique<Node>(element);
    new_head->m_next = std::move(m_head);
    if (new_head->m_next) {
      new_head->m_next->m_prev = new_head.get();
    }
    m_head = std::move(new_head);
    if (m_tail == nullptr) {
      m_tail = m_head.get();
    }
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    auto node = std::make_unique<Node>(std::forward<Args>(args)...);
    node->m_prev = m_tail;
    if (m_tail) {
      m_tail->m_next = std::move(node);
      m_tail = m_tail->m_next.get();
    } else {
      m_head = std::move(node);
      m_tail = m_head.get();
    }
  }

  template <class... Args>
  void emplace_front(Args&&... args) {
    auto new_head = std::make_unique<Node>(std::forward<Args>(args)...);
    new_head->m_next = std::move(m_head);
    if (new_head->m_next) {
      new_head->m_next->m_prev = new_head.get();
    }
    m_head = std::move(new_head);
    if (m_tail == nullptr) {
      m_tail = m_head.get();
    }
  }

  void pop_back() {
    if (m_tail) {
      if (Node* new_tail = m_tail->m_prev) {
        m_tail = new_tail;
        m_tail->m_next = nullptr;
      } else {
        // There's only one element
        m_head = nullptr;
        m_tail = nullptr;
      }
    }
  }

  void pop_front() {
    if (m_head) {
      m_head = std::move(m_head->m_next);
      if (m_head) {
        m_head->m_prev = nullptr;
      }
    }
  }

  iterator erase(const_iterator pos) {
    // let's treat special cases first
    if (pos == cend()) {
      return end();
    }

    if (pos.m_current == m_head.get()) {
      pop_front();
      return begin();
    }

    if (pos.m_current == m_tail) {
      pop_back();
      return end();
    }

    auto deleted_element = std::move(pos.m_current->m_prev->m_next);
    deleted_element->m_next->m_prev = deleted_element->m_prev;
    deleted_element->m_prev->m_next = std::move(deleted_element->m_next);
    return iterator{deleted_element->m_prev->m_next.get()};
  }

 private:
  std::unique_ptr<Node> m_head = nullptr;
  Node* m_tail = nullptr;
};


#pragma once

#include <memory>

template <class T>
class LinkedList {
 public:
  class Node {
    T m_element;
    std::unique_ptr<Node> m_next;

   public:
    explicit Node(T element) : m_element(std::move(element)) {}

    template <class... Args>
    Node(Args&&... args) : m_element(std::forward<Args>(args)...) {}

    Node(T element, std::unique_ptr<Node> next)
        : m_element(element), m_next(std::move(next)) {}

    auto get_next() const -> Node* { return m_next.get(); }

    auto get_element() -> T& { return m_element; }

    auto get_element() const -> const T& { return m_element; }

    auto insert_after(const T& new_element) {
      m_next = std::make_unique<Node>(new_element, std::move(m_next));
    }

    template <class... Args>
    auto emplace_after(Args&&... args) {
      auto next = std::make_unique<Node>(std::forward<Args>(args)...);
      next->m_next = std::move(m_next);
      m_next = std::move(next);
    }

    friend LinkedList;
  };

  LinkedList() = default;

  void push_back(T element) {
    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr != nullptr) {
      element_ptr = &(*element_ptr)->m_next;
    }
    *element_ptr = std::make_unique<Node>(std::move(element));
  }

  void push_front(T element) {
    m_head = std::make_unique<Node>(element, std::move(m_head));
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr != nullptr) {
      element_ptr = &(*element_ptr)->m_next;
    }
    *element_ptr = std::make_unique<Node>(std::forward<Args>(args)...);
  }

  template <class... Args>
  void emplace_front(Args&&... args) {
    auto new_head = std::make_unique<Node>(std::forward<Args>(args)...);
    new_head->m_next = std::move(m_head);
    m_head = std::move(new_head);
  }

  void remove(Node* node) {
    if (node == nullptr) {
      return;
    }

    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr) {
      if ((*element_ptr).get() == node) {
        *element_ptr = std::move((*element_ptr)->m_next);
        return;
      }
      element_ptr = &(*element_ptr)->m_next;
    }
  }

  auto tail() -> Node* {
    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr) {
      if ((*element_ptr)->m_next == nullptr) {
        return (*element_ptr).get();
      }
      element_ptr = &(*element_ptr)->m_next;
    }
    return nullptr;
  }

  auto head() -> Node* { return m_head.get(); }

  template <class U>
  auto walk(U action) {
    Node* ptr = m_head.get();
    while (ptr) {
      action(ptr->get_element());
      ptr = ptr->get_next();
    }
  }

  template <class U>
  auto find(U action) const -> Node* {
    Node* ptr = m_head.get();
    while (ptr) {
      if (action(ptr->get_element())) {
        return ptr;
      }
      ptr = ptr->get_next();
    }
    return nullptr;
  }

 private:
  std::unique_ptr<Node> m_head = nullptr;
};

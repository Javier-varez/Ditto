
#pragma once

#include <memory>

template <class T>
class LinkedList {
 public:
  class Node {
    T element;
    std::unique_ptr<Node> next;

   public:
    explicit Node(T element) : element(element) {}
    Node(T element, std::unique_ptr<Node> next)
        : element(element), next(std::move(next)) {}

    auto get_next() const -> Node* { return next.get(); }

    auto get_element() -> T& { return element; }

    friend LinkedList;
  };

  LinkedList() = default;

  void push_back(T&& element) {
    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr != nullptr) {
      element_ptr = &(*element_ptr)->next;
    }
    *element_ptr = std::make_unique<Node>(std::move(element));
  }

  void push_front(T element) {
    m_head = std::make_unique<Node>(element, std::move(m_head));
  }

  void remove(Node* node) {
    if (node == nullptr) {
      return;
    }

    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr) {
      if ((*element_ptr).get() == node) {
        *element_ptr = std::move((*element_ptr)->next);
        return;
      }
      element_ptr = &(*element_ptr)->next;
    }
  }

  auto tail() -> Node* {
    std::unique_ptr<Node>* element_ptr = &m_head;
    while (*element_ptr) {
      if ((*element_ptr)->next == nullptr) {
        return (*element_ptr).get();
      }
      element_ptr = &(*element_ptr)->next;
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

 private:
  std::unique_ptr<Node> m_head = nullptr;
};

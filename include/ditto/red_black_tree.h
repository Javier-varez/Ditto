#pragma once

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <utility>

#include "ditto/assert.h"
#include "ditto/non_null_ptr.h"
#include "ditto/optional.h"

namespace Ditto {

template <class K, class V>
class RedBlackTree {
 public:
  void insert(const K& key, const V& value);
  auto lookup(const K& key) -> Ditto::optional<Ditto::NonNullPtr<V>>;

  auto erase(const K& key) -> Ditto::optional<V>;

  auto depth() const -> std::uint32_t;

 private:
  enum class Color { RED = 0x00, BLACK = 0x01 };

  template <class T>
  class PointerAndColor {
   public:
    PointerAndColor(T* node, Color color = Color::RED) {
      // The node pointer must be aligned to 4 bytes
      const uintptr_t node_ptr = reinterpret_cast<std::uintptr_t>(node);
      DITTO_VERIFY((node_ptr & COLOR_MASK) == 0);

      m_value = node_ptr | static_cast<std::uintptr_t>(color);
    }

    Color color() const { return static_cast<Color>(m_value & 0x3); }
    T* pointer() const { return reinterpret_cast<T*>(m_value & PTR_MASK); }

    void set_pointer(T* node) {
      const auto ptr = reinterpret_cast<std::uintptr_t>(node);
      DITTO_VERIFY((ptr & COLOR_MASK) == 0);
      m_value &= COLOR_MASK;
      m_value |= ptr;
    }

    void set_color(Color color) {
      const auto color_val = static_cast<std::uintptr_t>(color);
      m_value &= PTR_MASK;
      m_value |= color_val;
    }

    void swap_color() {
      if (color() == Color::BLACK) {
        set_color(Color::RED);
      } else {
        set_color(Color::BLACK);
      }
    }

   private:
    std::uintptr_t m_value;
    constexpr static std::uintptr_t COLOR_MASK = std::uintptr_t{0x03};
    constexpr static std::uintptr_t PTR_MASK = ~COLOR_MASK;
  };

  struct Node {
    PointerAndColor<Node> color;
    K key;
    V value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(const K& key, const V& value, Node* parent)
        : key(key), value(value), color(parent, Color::RED) {}
  };

  std::unique_ptr<Node> m_root;

  void fixup_insertion(Ditto::NonNullPtr<Node> node);
  void rotate_right(Ditto::NonNullPtr<Node> node);
  void rotate_left(Ditto::NonNullPtr<Node> node);
  static auto depth_from_node(Node* node, std::uint32_t current_depth)
      -> std::uint32_t;
};  // namespace Ditto

template <class K, class V>
inline void RedBlackTree<K, V>::insert(const K& key, const V& value) {
  Ditto::NonNullPtr<std::unique_ptr<Node>> node_double_ptr = &m_root;
  Node* parent = nullptr;

  while (*node_double_ptr) {
    std::unique_ptr<Node>& node_ptr = *node_double_ptr;
    if (node_ptr->key == key) {
      node_ptr->value = value;
      return;
    }

    parent = node_ptr.get();
    if (key < node_ptr->key) {
      node_double_ptr = &node_ptr->left;
    } else {
      node_double_ptr = &node_ptr->right;
    }
  }

  *node_double_ptr = std::make_unique<Node>(key, value, parent);

  // Restructure nodes
  fixup_insertion(node_double_ptr->get());
}

template <class K, class V>
inline auto RedBlackTree<K, V>::lookup(const K& key)
    -> Ditto::optional<Ditto::NonNullPtr<V>> {
  Node* node = m_root.get();

  while (node != nullptr) {
    if (node->key == key) {
      return Ditto::optional<Ditto::NonNullPtr<V>>{
          Ditto::NonNullPtr<V>{&node->value}};
    } else if (key < node->key) {
      node = node->left.get();
    } else {
      node = node->right.get();
    }
  }
  return {};
}

template <class K, class V>
void RedBlackTree<K, V>::fixup_insertion(Ditto::NonNullPtr<Node> node) {
  auto parent = node->color.pointer();
  if (parent == nullptr) {
    // This is the root, which should always be black
    node->color.set_color(Color::BLACK);
  } else if (parent->color.color() == Color::BLACK) {
    // Already good, since parent is black!
    return;
  } else {
    auto grandparent = parent->color.pointer();
    if (grandparent->left.get() == parent) {
      auto uncle = grandparent->right.get();
      if (uncle && (uncle->color.color() == Color::RED)) {
        uncle->color.swap_color();
        parent->color.swap_color();
        grandparent->color.swap_color();
        fixup_insertion(grandparent);
      } else {
        // Parent is left and uncle is black
        if (parent->right.get() == node.get()) {
          rotate_left(parent);
        }
        rotate_right(grandparent);
        grandparent->color.swap_color();
        parent->color.swap_color();
      }
    } else {
      auto uncle = grandparent->left.get();
      if (uncle && (uncle->color.color() == Color::RED)) {
        uncle->color.swap_color();
        parent->color.swap_color();
        grandparent->color.swap_color();
        fixup_insertion(grandparent);
      } else {
        // Parent is right and uncle is black
        if (parent->left.get() == node.get()) {
          rotate_right(parent);
        }
        rotate_left(grandparent);
        grandparent->color.swap_color();
        parent->color.swap_color();
      }
    }
  }
}

template <class K, class V>
void RedBlackTree<K, V>::rotate_right(Ditto::NonNullPtr<Node> node) {
  auto parent = node->color.pointer();
  std::unique_ptr<Node> new_node = std::move(node->left);
  std::unique_ptr<Node> prev_right = std::move(new_node->right);

  std::unique_ptr<Node>* parent_ref = nullptr;
  if (parent == nullptr) {
    parent_ref = &m_root;
  } else if (parent->right.get() == node.get()) {
    parent_ref = &parent->right;
  } else {
    parent_ref = &parent->left;
  }

  node->color.set_pointer(new_node.get());
  new_node->right = std::move(*parent_ref);

  new_node->color.set_pointer(parent);
  *parent_ref = std::move(new_node);

  node->left = std::move(prev_right);
  if (node->left) {
    node->left->color.set_pointer(node.get());
  }
}

template <class K, class V>
void RedBlackTree<K, V>::rotate_left(Ditto::NonNullPtr<Node> node) {
  auto parent = node->color.pointer();
  std::unique_ptr<Node> new_node = std::move(node->right);
  std::unique_ptr<Node> prev_left = std::move(new_node->left);

  std::unique_ptr<Node>* parent_ref = nullptr;
  if (parent == nullptr) {
    parent_ref = &m_root;
  } else if (parent->right.get() == node.get()) {
    parent_ref = &parent->right;
  } else {
    parent_ref = &parent->left;
  }

  node->color.set_pointer(new_node.get());
  new_node->left = std::move(*parent_ref);

  new_node->color.set_pointer(parent);
  *parent_ref = std::move(new_node);

  node->right = std::move(prev_left);
  if (node->right) {
    node->right->color.set_pointer(node.get());
  }
}

template <class K, class V>
auto RedBlackTree<K, V>::depth_from_node(Node* node, uint32_t current_depth)
    -> std::uint32_t {
  uint32_t depth = current_depth;

  if (node) {
    depth++;

    if (node->left && node->right) {
      return std::max(depth_from_node(node->left.get(), depth),
                      depth_from_node(node->right.get(), depth));
    }

    if (node->left) {
      depth = depth_from_node(node->left.get(), depth);
    }

    if (node->right) {
      depth = depth_from_node(node->right.get(), depth);
    }
  }

  return depth;
}

template <class K, class V>
auto RedBlackTree<K, V>::depth() const -> std::uint32_t {
  return depth_from_node(m_root.get(), 0);
}

}  // namespace Ditto

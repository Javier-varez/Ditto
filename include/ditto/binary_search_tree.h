#ifndef DITTO_BINARY_SEARCH_TREE_H_
#define DITTO_BINARY_SEARCH_TREE_H_

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <utility>

#include "ditto/non_null_ptr.h"
#include "ditto/optional.h"

namespace Ditto {

template <class K, class V>
class BinarySearchTree {
 public:
  void insert(const K& key, const V& value);
  auto lookup(const K& key) -> Ditto::optional<Ditto::NonNullPtr<V>>;

  auto erase(const K& key) -> Ditto::optional<V>;

  [[nodiscard]] auto depth() const -> std::uint32_t;

 private:
  struct Node {
    K key;
    V value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(const K& key, const V& value) : key(key), value(value) {}
  };

  std::unique_ptr<Node> m_root;

  static void insert_node(Ditto::NonNullPtr<std::unique_ptr<Node>> root,
                          std::unique_ptr<Node> new_node);
  static auto depth_from_node(Node* node, std::uint32_t current_depth)
      -> std::uint32_t;
};

template <class K, class V>
inline void BinarySearchTree<K, V>::insert(const K& key, const V& value) {
  Ditto::NonNullPtr<std::unique_ptr<Node>> node_double_ptr = &m_root;

  while (*node_double_ptr) {
    std::unique_ptr<Node>& node_ptr = *node_double_ptr;
    if (node_ptr->key == key) {
      node_ptr->value = value;
      return;
    }

    if (key < node_ptr->key) {
      node_double_ptr = &node_ptr->left;
    } else {
      node_double_ptr = &node_ptr->right;
    }
  }

  *node_double_ptr = std::make_unique<Node>(key, value);
}

template <class K, class V>
inline auto BinarySearchTree<K, V>::lookup(const K& key)
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
auto BinarySearchTree<K, V>::erase(const K& key) -> Ditto::optional<V> {
  Ditto::NonNullPtr<std::unique_ptr<Node>> node_double_ptr = &m_root;

  while (*node_double_ptr != nullptr) {
    std::unique_ptr<Node>& node_ptr = *node_double_ptr;

    if (node_ptr->key == key) {
      std::unique_ptr<Node> removed_node = std::move(node_ptr);
      if (removed_node->left == nullptr) {
        node_ptr = std::move(removed_node->right);
      } else if (removed_node->right == nullptr) {
        node_ptr = std::move(removed_node->left);
      } else {
        // They are both valid. In this case we take the left node as the new
        // parent and then insert the right node into the right subtree
        //
        node_ptr = std::move(removed_node->left);
        insert_node(&node_ptr, std::move(removed_node->right));
      }

      return Ditto::optional<V>{std::move(removed_node->value)};
    } else if (key < node_ptr->key) {
      node_double_ptr = &node_ptr->left;
    } else {
      node_double_ptr = &node_ptr->right;
    }
  }

  return {};
}

template <class K, class V>
void BinarySearchTree<K, V>::insert_node(
    Ditto::NonNullPtr<std::unique_ptr<Node>> root,
    std::unique_ptr<Node> new_node) {
  Ditto::NonNullPtr<std::unique_ptr<Node>> node_double_ptr = root;

  while (*node_double_ptr) {
    std::unique_ptr<Node>& node_ptr = *node_double_ptr;
    if (new_node->key < node_ptr->key) {
      node_double_ptr = &node_ptr->left;
    } else {
      node_double_ptr = &node_ptr->right;
    }
  }

  *node_double_ptr = std::move(new_node);
}

template <class K, class V>
auto BinarySearchTree<K, V>::depth_from_node(Node* node, uint32_t current_depth)
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
auto BinarySearchTree<K, V>::depth() const -> std::uint32_t {
  return depth_from_node(m_root.get(), 0);
}

}  // namespace Ditto

#endif  // DITTO_BINARY_SEARCH_TREE_H_

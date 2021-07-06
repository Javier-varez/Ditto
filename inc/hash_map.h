
#pragma once

#include <vector>

#include "linked_list.h"
#include "pair.h"

template <class K, class V>
class HashMap {
 public:
  HashMap() = default;
  explicit HashMap(std::size_t capacity)
      : m_capacity(capacity),
        m_elements(new LinkedList<Pair<K, V>>[capacity]){};

  ~HashMap() { delete[] m_elements; }

  auto operator[](const K& key) -> V& {
    auto& node = search_or_reserve_slot(m_elements[hash_key(key)], key);
    return node.get_element().right();
  }

  auto at(const K& key) const -> const V* {
    auto* node = search_slot(m_elements[hash_key(key)], key);
    if (node == nullptr) {
      return nullptr;
    }
    return &node->get_element().right();
  }

  auto at(const K& key) -> V* {
    auto* node = search_slot(m_elements[hash_key(key)], key);
    if (node == nullptr) {
      return nullptr;
    }
    return &node->get_element().right();
  }

  auto erase(const K& key) {
    auto* node = search_slot(m_elements[hash_key(key)], key);
    if (node != nullptr) {
      m_elements[hash_key(key)].remove(node);
    }
  }

 private:
  constexpr static std::size_t DEFAULT_CAPACITY = 1024;
  std::size_t m_capacity = DEFAULT_CAPACITY;
  LinkedList<Pair<K, V>>* m_elements{
      new LinkedList<Pair<K, V>>[DEFAULT_CAPACITY]};

  auto search_slot(const LinkedList<Pair<K, V>>& list, const K& key) const ->
      typename LinkedList<Pair<K, V>>::Node* {
    auto element = list.find(
        [&key](const Pair<K, V>& element) { return element.left() == key; });
    return element;
  }

  auto search_or_reserve_slot(LinkedList<Pair<K, V>>& list, const K& key) ->
      typename LinkedList<Pair<K, V>>::Node& {
    auto element = search_slot(list, key);
    if (element == nullptr) {
      list.push_back(Pair<K, V>(key, V{}));
      element = list.tail();
    }
    return *element;
  }

  // These are random hashing functions, I should invest some more time into
  // improving them.
  template <std::enable_if_t<std::is_integral<K>::value, bool> = false>
  auto hash_key(const K& key) const -> std::size_t {
    uint64_t hash = key;
    hash ^= 0xDEADC0DE;
    hash ^= key << 13;
    hash += key >> 1;
    hash += key << 22;
    hash ^= key << 3;
    hash ^= ~(key << 2);
    hash += key << 8;
    hash += key >> 20;
    hash ^= key << 7;
    return hash % m_capacity;
  }

  friend class HashMapTest;
};

#ifndef DITTO_HASH_MAP_H_
#define DITTO_HASH_MAP_H_

#include <algorithm>
#include <cstddef>

#include "ditto/hash.h"
#include "ditto/linked_list.h"
#include "ditto/pair.h"

namespace Ditto {

template <class K, class V, class H = SimpleHasher>
requires Hashable<H, K> class HashMap {
 public:
  HashMap() = default;
  explicit HashMap(std::size_t capacity)
      : m_capacity(capacity),
        m_elements(new LinkedList<Pair<K, V>>[capacity]){};

  ~HashMap() { delete[] m_elements; }

  [[nodiscard]] static auto calculate_hash(const K& key) -> std::uint32_t {
    H hasher;
    hasher.hash(key);
    return hasher.finish();
  }

  [[nodiscard]] auto operator[](const K& key) -> V& {
    auto node = search_or_reserve_slot(
        m_elements[calculate_hash(key) % m_capacity], key);
    return node->right();
  }

  [[nodiscard]] auto at(const K& key) const -> const V* {
    const auto& list = m_elements[calculate_hash(key) % m_capacity];
    auto iter = search_slot(list, key);
    if (iter == list.cend()) {
      return nullptr;
    }
    return &iter->right();
  }

  [[nodiscard]] auto at(const K& key) -> V* {
    auto& list = m_elements[calculate_hash(key) % m_capacity];
    auto iter = search_slot(list, key);
    if (iter == list.cend()) {
      return nullptr;
    }
    return &iter->right();
  }

  auto erase(const K& key) {
    auto& list = m_elements[calculate_hash(key) % m_capacity];
    auto iter = search_slot(list, key);
    if (iter != list.cend()) {
      list.erase(iter);
    }
  }

 private:
  constexpr static std::size_t DEFAULT_CAPACITY = 1024;
  std::size_t m_capacity = DEFAULT_CAPACITY;
  LinkedList<Pair<K, V>>* m_elements{
      new LinkedList<Pair<K, V>>[DEFAULT_CAPACITY]};

  auto search_slot(const LinkedList<Pair<K, V>>& list, const K& key) const ->
      typename LinkedList<Pair<K, V>>::const_iterator {
    auto iter = std::find_if(
        list.cbegin(), list.cend(),
        [&key](const Pair<K, V>& pair) { return pair.left() == key; });
    return iter;
  }

  auto search_slot(LinkedList<Pair<K, V>>& list, const K& key) ->
      typename LinkedList<Pair<K, V>>::iterator {
    auto iter = std::find_if(
        list.begin(), list.end(),
        [&key](const Pair<K, V>& pair) { return pair.left() == key; });
    return iter;
  }

  auto search_or_reserve_slot(LinkedList<Pair<K, V>>& list, const K& key) ->
      typename LinkedList<Pair<K, V>>::iterator {
    auto element = search_slot(list, key);
    if (element == list.cend()) {
      list.push_back(Pair<K, V>(key, V{}));
      element = list.back_iter();
    }
    return element;
  }

  friend class HashMapTest;
};

}  // namespace Ditto

#endif  // DITTO_HASH_MAP_H_

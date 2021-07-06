
#pragma once

#include <cstdint>

template <class K, class V, std::size_t CAPACITY>
class LinearMap {
 public:
  LinearMap() = default;

  auto operator[](const K& key) -> V& {
    auto& bucket = *search_or_reserve_slot(key);
    return *bucket.value();
  }

  auto at(const K& key) const -> const V* {
    auto* bucket = search_slot(key);
    if (bucket == nullptr) {
      return nullptr;
    }
    return bucket->value();
  }

  auto at(const K& key) -> V* {
    auto* bucket = search_slot(key);
    if (bucket == nullptr) {
      return nullptr;
    }
    return bucket->value();
  }

 private:
  struct Bucket {
    K key;
    alignas(V) std::uint8_t value_memory[sizeof(V)];
    bool used = false;

    auto value() -> V* { return reinterpret_cast<V*>(value_memory); }
    auto value() const -> const V* {
      return reinterpret_cast<const V*>(value_memory);
    }
  };
  Bucket m_buckets[CAPACITY];

  auto search_slot(const K& key) const -> const Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].used) {
        if (m_buckets[i].key == key) {
          return &m_buckets[i];
        }
      }
    }
    return nullptr;
  }

  auto search_slot(const K& key) -> Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].used) {
        if (m_buckets[i].key == key) {
          return &m_buckets[i];
        }
      }
    }
    return nullptr;
  }

  auto search_or_reserve_slot(const K& key) -> Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].used) {
        if (m_buckets[i].key == key) {
          return &m_buckets[i];
        }
      }
    }

    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (!m_buckets[i].used) {
        m_buckets[i].used = true;
        m_buckets[i].key = key;
        return &m_buckets[i];
      }
    }
    return nullptr;
  }

  friend class LinearMapTest;
};

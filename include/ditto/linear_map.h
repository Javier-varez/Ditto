#ifndef DITTO_LINEAR_MAP_H_
#define DITTO_LINEAR_MAP_H_

#include <cstddef>
#include <cstdint>
#include <new>
#include <optional>
#include <utility>

namespace Ditto {

template <class K, class V, std::size_t CAPACITY>
class LinearMap {
 public:
  LinearMap() = default;

  auto operator[](const K& key) -> V& {
    auto& bucket = *search_or_reserve_slot(key);
    return bucket.value();
  }

  bool contains(const K& key) const { return search_slot(key) != nullptr; }

  auto at(const K& key) const -> std::optional<V> {
    auto* bucket = search_slot(key);
    if (bucket == nullptr) {
      return std::optional<V>{};
    }
    return std::optional{bucket->value()};
  }

  template <class... Args>
  auto emplace(const K& key, Args&&... args) {
    auto& bucket = *search_or_reserve_slot(key);
    bucket.key() = key;
    bucket.emplace(std::forward<Args>(args)...);
  }

  auto erase(const K& key) {
    auto* bucket = search_slot(key);
    if (bucket != nullptr) {
      bucket->set_used(false);
      bucket->key().~K();
      bucket->value().~V();
    }
  }

  ~LinearMap() {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].is_used()) {
        m_buckets[i].set_used(false);
        m_buckets[i].key().~K();
        m_buckets[i].value().~V();
      }
    }
  }

 private:
  class Bucket {
   public:
    auto value() -> V& { return *reinterpret_cast<V*>(value_memory); }
    auto key() -> K& { return *reinterpret_cast<K*>(key_memory); }
    auto value() const -> const V& {
      return *reinterpret_cast<const V*>(value_memory);
    }
    auto key() const -> const K& {
      return *reinterpret_cast<const K*>(key_memory);
    }

    template <class... Args>
    auto emplace(Args&&... args) {
      // Construct value in place
      new (value_memory) V{args...};
    }

    [[nodiscard]] auto is_used() const -> bool { return m_used; }
    auto set_used(bool val) { m_used = val; }

   private:
    alignas(K) std::uint8_t key_memory[sizeof(K)];
    alignas(V) std::uint8_t value_memory[sizeof(V)];

    bool m_used = false;
  };
  Bucket m_buckets[CAPACITY];

  auto search_slot(const K& key) const -> const Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].is_used()) {
        if (m_buckets[i].key() == key) {
          return &m_buckets[i];
        }
      }
    }
    return nullptr;
  }

  auto search_slot(const K& key) -> Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].is_used()) {
        if (m_buckets[i].key() == key) {
          return &m_buckets[i];
        }
      }
    }
    return nullptr;
  }

  auto search_or_reserve_slot(const K& key) -> Bucket* {
    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (m_buckets[i].is_used()) {
        if (m_buckets[i].key() == key) {
          return &m_buckets[i];
        }
      }
    }

    for (std::size_t i = 0; i < CAPACITY; i++) {
      if (!m_buckets[i].is_used()) {
        m_buckets[i].set_used(true);
        m_buckets[i].key() = key;
        return &m_buckets[i];
      }
    }
    return nullptr;
  }

  friend class LinearMapTest;
};

}  // namespace Ditto

#endif  // DITTO_LINEAR_MAP_H_

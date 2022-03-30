#ifndef DITTO_FIXED_FLAT_MAP_H_
#define DITTO_FIXED_FLAT_MAP_H_

#include "ditto/hash.h"
#include "ditto/pair.h"
#include "ditto/result.h"

namespace Ditto {

// TODO(javier-varez): Monitor fill level and error out if exceeds maximum
// When the fill level is too high the map won't work at all.

// TODO(javier-varez): Figure out how to deal with deleted entries creating
// loops A possible solution is to link nodes like a linked list, but statically
// allocated in the same storage space

/**
 * @brief Fixed-size Hash Map that is statically-allocated and uses open
 *        addressing for dealing with hash collisions in the table.
 *
 * This is usable/intended for embedded targets where no dynamic memory can be
 * used but we still would like to have O(1) lookup, insertion and removal, (as
 * long as the fill factor is not too high) with respect to the capacity of the
 * map.
 */
template <class K, class V, uint32_t CAPACITY, class H = SimpleHasher>
requires Hashable<H, K> class FixedFlatMap {
 public:
  enum class Error { KeyAlreadyUsed, KeyNotFound };

  /**
   * @brief constructs an empty `FixedFlatMap`. The capacity is fixed and given
   * by the template argument CAPACITY.
   *
   * Note that insertion/lookup complexity grows and performance shrinks as the
   * load factor grows.
   *
   * Since this container is not resizeable, it is best used when the load
   * factor is kept relatively small.
   */
  FixedFlatMap() = default;

  //! Errors out if the element is already inserted
  template <class... T>
  Ditto::Result<V*, Error> try_emplace(const K& key, T... args) {
    const auto hash = calculate_hash(key);
    const auto slot = find_slot(hash, key);
    auto& meta = m_metadata[slot];
    if (meta.isUsed()) {
      return Error::KeyAlreadyUsed;
    }

    auto new_element =
        new (&m_storage[slot]) KvPair{key, std::forward<T>(args)...};
    meta.setUsed(hash);

    return &new_element->right();
  }

  Ditto::Result<V*, Error> operator[](const K& key) {
    const auto hash = calculate_hash(key);
    const auto slot = find_slot(hash, key);
    auto& meta = m_metadata[slot];
    if (!meta.isUsed()) {
      return Error::KeyNotFound;
    }

    auto kv_pair = reinterpret_cast<KvPair*>(&m_storage[slot]);
    return &kv_pair->right();
  }

  Ditto::Result<void, Error> remove(const K& key) {
    const auto hash = calculate_hash(key);
    const auto slot = find_slot(hash, key);
    auto& meta = m_metadata[slot];
    if (!meta.isUsed()) {
      return Error::KeyNotFound;
    }

    meta.setDeleted();
    auto kv_pair = reinterpret_cast<KvPair*>(m_storage[slot]);
    kv_pair->~KvPair();

    return Ditto::Result<void, Error>::ok();
  }

 private:
  class Meta {
   public:
    Meta() : m_inner(EMPTY_FLAG) {}
    explicit Meta(std::uint32_t hash) : m_inner(hash & HASH_MASK) {}

    void setUsed(std::uint32_t hash) { m_inner = hash & HASH_MASK; }
    void setEmpty() { m_inner = EMPTY_FLAG; }
    void setDeleted() { m_inner = EMPTY_FLAG | DELETED_FLAG; }

    [[nodiscard]] bool isDeleted() const {
      return (m_inner & (EMPTY_FLAG | DELETED_FLAG)) ==
             (EMPTY_FLAG | DELETED_FLAG);
    }

    [[nodiscard]] bool isEmpty() const {
      return (m_inner & (EMPTY_FLAG | DELETED_FLAG)) == EMPTY_FLAG;
    }

    [[nodiscard]] bool isUsed() const { return (m_inner & EMPTY_FLAG) == 0; }

    [[nodiscard]] bool matchesHash(std::uint32_t hash) const {
      return isUsed() && (m_inner == (hash & HASH_MASK));
    }

   private:
    constexpr static std::uint32_t EMPTY_FLAG = 1 << 31;
    // The deleted flag is only valid if the empty flag is true
    constexpr static std::uint32_t DELETED_FLAG = 1 << 30;
    constexpr static std::uint32_t HASH_MASK = ~EMPTY_FLAG;

    uint32_t m_inner;
  };

  template <class T>
  requires Hashable<H, T> static auto calculate_hash(const T& val)
      -> std::uint32_t {
    H hasher;
    hasher.hash(val);
    return hasher.finish();
  }

  auto find_slot(const std::uint32_t hash, const K& key) -> std::size_t {
    auto current_hash = hash;

    std::size_t slot = SIZE_MAX;

    for (;;) {
      const auto current_slot = current_hash % CAPACITY;
      if (m_metadata[current_slot].isEmpty()) {
        // Found insertion point!
        if (slot == SIZE_MAX) {
          slot = current_slot;
        }
        break;
      } else if (m_metadata[current_slot].isUsed()) {
        if (m_metadata[current_slot].matchesHash(hash)) {
          // Also make sure that the key matches
          auto kv_pair =
              reinterpret_cast<const KvPair*>(&m_storage[current_slot]);
          if (kv_pair->left() == key) {
            slot = current_slot;
            break;
          }
          // There was a hash collision, we try again
          current_hash = calculate_hash(current_hash);
        } else {
          // Slot does not match the hash... We keep going
          current_hash = calculate_hash(current_hash);
        }
      } else if (m_metadata[current_slot].isDeleted()) {
        // We store the deleted slot, but don't use it until we find an empty
        // slot
        if (slot == SIZE_MAX) {
          slot = current_slot;
        }

        current_hash = calculate_hash(current_hash);
      }
    }

    DITTO_VERIFY(slot != SIZE_MAX);
    return slot;
  }

  using KvPair = Ditto::Pair<K, V>;
  using KvPairStorage =
      std::aligned_storage_t<sizeof(KvPair), std::alignment_of_v<KvPair>>;

  std::array<KvPairStorage, CAPACITY> m_storage;
  std::array<Meta, CAPACITY> m_metadata;
};

}  // namespace Ditto

#endif  // DITTO_FIXED_FLAT_MAP_H_

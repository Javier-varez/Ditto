
#include <cstdint>
#include <type_traits>

namespace ATE {

class Hash {
 public:
  // These are random hashing functions, I should invest some more time into
  // improving them.
  template <class K, std::enable_if_t<std::is_integral_v<K>, bool> = false>
  static auto calculate(const K& key) -> std::size_t {
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
    return hash;
  }
};

}  // namespace ATE

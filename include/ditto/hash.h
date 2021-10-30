
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Ditto {

namespace Hash {

template <class T>
auto calculate(const T& key) -> std::size_t;

auto calculate(std::uint64_t key) -> std::size_t;
auto calculate(std::uint32_t key) -> std::size_t;
auto calculate(std::uint16_t key) -> std::size_t;
auto calculate(std::uint8_t key) -> std::size_t;
auto calculate(std::int64_t key) -> std::size_t;
auto calculate(std::int32_t key) -> std::size_t;
auto calculate(std::int16_t key) -> std::size_t;
auto calculate(std::int8_t key) -> std::size_t;

};  // namespace Hash

}  // namespace Ditto

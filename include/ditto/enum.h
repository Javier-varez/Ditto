#ifndef DITTO_ENUM_H_
#define DITTO_ENUM_H_

#include <algorithm>
#include <type_traits>
#include <vector>

#include "ditto/assert.h"
#include "ditto/optional.h"
#include "ditto/type_traits.h"

namespace Ditto {

template <ScopedEnum auto VariantValue, typename VariantType>
struct EnumVariant final {
  constexpr static decltype(VariantValue) enum_value = VariantValue;
  using type = VariantType;

  template <typename... Args>
  static void construct(void* ptr, Args&&... args) noexcept {
    new (ptr) VariantType{std::forward<Args>(args)...};
  }

  static void copyConstruct(void* dest, const void* source) noexcept
      requires(std::is_copy_constructible_v<VariantType>) {
    const VariantType& sourceVar =
        *reinterpret_cast<const VariantType*>(source);
    new (dest) VariantType{sourceVar};
  }

  static void moveConstruct(void* dest, void* source) noexcept
      requires(std::is_move_constructible_v<VariantType>) {
    VariantType& sourceVar = *reinterpret_cast<VariantType*>(source);
    new (dest) VariantType{std::move(sourceVar)};
  }

  static void moveConstruct(void* dest, void* source) noexcept
      requires(!std::is_move_constructible_v<VariantType> &&
               std::is_copy_constructible_v<VariantType>) {
    copyConstruct(dest, source);
  }

  static void copyAssign(void* dest, const void* source) noexcept
      requires(std::is_copy_assignable_v<VariantType>) {
    const VariantType& sourceVar =
        *reinterpret_cast<const VariantType*>(source);
    VariantType& destVar = *reinterpret_cast<VariantType*>(dest);
    destVar = sourceVar;
  }

  static void moveAssign(void* dest, void* source) noexcept
      requires(std::is_move_assignable_v<VariantType>) {
    VariantType& sourceVar = *reinterpret_cast<VariantType*>(source);
    VariantType& destVar = *reinterpret_cast<VariantType*>(source);
    destVar = std::move(sourceVar);
  }

  static void moveAssign(void* dest, void* source) noexcept
      requires(!std::is_move_assignable_v<VariantType> &&
               std::is_copy_assignable_v<VariantType>) {
    copyAssign(dest, source);
  }

  static void destroy(void* ptr) noexcept
      requires(!std::is_trivially_destructible_v<VariantType>) {
    reinterpret_cast<VariantType*>(ptr)->~VariantType();
  }

  static void destroy(void* ptr) noexcept
      requires(std::is_trivially_destructible_v<VariantType>) {}
};

namespace enumeration {
namespace detail {

// Counts the number of times the given variant appears in the variant list.
template <ScopedEnum auto VariantValue, typename FirstVariant,
          typename... Others>
consteval auto validate_single_variant(size_t count = 0) -> size_t {
  if constexpr (VariantValue == FirstVariant::enum_value) {
    count++;
  }

  if constexpr (sizeof...(Others) > 0) {
    return validate_single_variant<VariantValue, Others...>(count);
  }

  return count;
};

template <typename... Variants>
consteval auto validate_variants() -> bool {
  // Each variant must appear 1 time
  return ((validate_single_variant<Variants::enum_value, Variants...>() == 1) &&
          ...);
}

template <ScopedEnum auto VariantValue, typename... Variants>
struct FindVariant;

template <ScopedEnum auto VariantValue, typename V, typename... Others>
struct FindVariant<VariantValue, EnumVariant<VariantValue, V>, Others...>
    final {
  using type = EnumVariant<VariantValue, V>;
};

template <ScopedEnum auto VariantValue, ScopedEnum auto OtherVariantEnum,
          typename V, typename... Others>
struct FindVariant<VariantValue, EnumVariant<OtherVariantEnum, V>, Others...>
    final {
  using type = typename FindVariant<VariantValue, Others...>::type;
};

template <ScopedEnum auto VariantValue, typename... Variants>
using find_variant_t = typename FindVariant<VariantValue, Variants...>::type;

template <ScopedEnum auto VariantValue, typename FirstVariant,
          typename... Variants>
consteval auto is_fat_variant() -> bool {
  if constexpr (VariantValue == FirstVariant::enum_value) {
    return true;
  } else if constexpr (sizeof...(Variants) > 0) {
    return is_fat_variant<VariantValue, Variants...>();
  } else {
    return false;
  }
}

template <ScopedEnum T, typename... Variants>
struct EnumVariants;

template <ScopedEnum T>
struct EnumVariants<T> final {
  static void copyConstruct(T variant, void* ptr, const void* src) noexcept {}
  static void moveConstruct(T variant, void* ptr, void* src) noexcept {}
  static void copyAssign(T variant, void* ptr, const void* src) noexcept {}
  static void moveAssign(T variant, void* ptr, void* src) noexcept {}
  static void destroy(T variant, void* ptr) noexcept {}
};

template <ScopedEnum T, typename FirstVariant, typename... Others>
struct EnumVariants<T, FirstVariant, Others...> final {
  static void copyConstruct(T variant, void* ptr, const void* src) noexcept {
    if (variant == FirstVariant::enum_value) {
      FirstVariant::copyConstruct(ptr, src);
      return;
    }
    EnumVariants<T, Others...>::copyConstruct(variant, ptr, src);
  }
  static void moveConstruct(T variant, void* ptr, void* src) noexcept {
    if (variant == FirstVariant::enum_value) {
      FirstVariant::moveConstruct(ptr, src);
      return;
    }
    EnumVariants<T, Others...>::moveConstruct(variant, ptr, src);
  }
  static void copyAssign(T variant, void* ptr, const void* src) noexcept {
    if (variant == FirstVariant::enum_value) {
      FirstVariant::copyAssign(ptr, src);
      return;
    }
    EnumVariants<T, Others...>::copyAssign(variant, ptr, src);
  }
  static void moveAssign(T variant, void* ptr, void* src) noexcept {
    if (variant == FirstVariant::enum_value) {
      FirstVariant::moveAssign(ptr, src);
      return;
    }
    EnumVariants<T, Others...>::moveAssign(variant, ptr, src);
  }
  static void destroy(T variant, void* ptr) noexcept {
    if (variant == FirstVariant::enum_value) {
      FirstVariant::destroy(ptr);
      return;
    }
    EnumVariants<T, Others...>::destroy(variant, ptr);
  }
};

}  // namespace detail

template <ScopedEnum auto>
struct InPlace final {
  explicit InPlace() = default;
};

template <auto VariantEnum, typename... Variants>
concept FatVariant =
    enumeration::detail::is_fat_variant<VariantEnum, Variants...>();

template <auto VariantEnum, typename... Variants>
concept NotFatVariant =
    !enumeration::detail::is_fat_variant<VariantEnum, Variants...>();

}  // namespace enumeration

template <ScopedEnum T, typename... Variants>
class Enum final {
 public:
  template <T VariantValue>
  requires(enumeration::NotFatVariant<VariantValue, Variants...>) explicit Enum(
      enumeration::InPlace<VariantValue>) noexcept
      : m_variant(VariantValue) {}

  template <T VariantValue, typename... Args>
  requires(enumeration::FatVariant<VariantValue, Variants...>) explicit Enum(
      enumeration::InPlace<VariantValue>, Args&&... args) noexcept {
    using Variant =
        typename enumeration::detail::find_variant_t<VariantValue, Variants...>;
    m_variant = Variant::enum_value;
    Variant::construct(&m_storage, std::forward<Args>(args)...);
  }

  Enum(const Enum& other) noexcept
      requires((std::is_copy_constructible_v<typename Variants::type> && ...))
      : m_variant(other.m_variant) {
    enumeration::detail::EnumVariants<T, Variants...>::copyConstruct(
        m_variant, &m_storage, &other.m_storage);
  }

  Enum(const Enum& other) noexcept
      requires(!(std::is_copy_constructible_v<typename Variants::type> &&
                 ...)) = delete;

  // Move falls back to copy if not supported by the specific type
  Enum(Enum&& other) noexcept
      requires(((std::is_move_constructible_v<typename Variants::type> ||
                 std::is_copy_constructible_v<typename Variants::type>)&&...))
      : m_variant(other.m_variant) {
    enumeration::detail::EnumVariants<T, Variants...>::moveConstruct(
        m_variant, &m_storage, &other.m_storage);
  }

  Enum(Enum&& other) noexcept requires(!(
      ((std::is_move_constructible_v<typename Variants::type> ||
        std::is_copy_constructible_v<typename Variants::type>)&&...))) = delete;

  auto operator=(const Enum& other) noexcept
      -> Enum& requires((std::is_copy_assignable_v<typename Variants::type> &&
                         ...)) {
    if (this != &other) {
      enumeration::detail::EnumVariants<T, Variants...>::destroy(m_variant,
                                                                 &m_storage);
      m_variant = other.m_variant;
      enumeration::detail::EnumVariants<T, Variants...>::copyAssign(
          m_variant, &m_storage, &other.m_storage);
    }
    return *this;
  }

  auto operator=(const Enum& other) noexcept
      -> Enum& requires(!(std::is_copy_assignable_v<typename Variants::type> &&
                          ...)) = delete;

  auto operator=(Enum&& other) noexcept -> Enum& requires(
      ((std::is_move_assignable_v<typename Variants::type> ||
        std::is_copy_assignable_v<typename Variants::type>)&&...)) {
    if (this != &other) {
      enumeration::detail::EnumVariants<T, Variants...>::destroy(m_variant,
                                                                 &m_storage);
      m_variant = other.m_variant;
      enumeration::detail::EnumVariants<T, Variants...>::moveAssign(
          m_variant, &m_storage, &other.m_storage);
    }
    return *this;
  }

  auto operator=(Enum&& other) noexcept -> Enum& requires(
      !(((std::is_move_assignable_v<typename Variants::type> ||
          std::is_copy_assignable_v<typename Variants::type>)&&...))) = delete;

  // For non-trivially-destructible variants
  ~Enum() noexcept
      requires(!(std::is_trivially_destructible_v<typename Variants::type> &&
                 ...)) {
    enumeration::detail::EnumVariants<T, Variants...>::destroy(m_variant,
                                                               &m_storage);
  }

  // Destructor for trivially destructible errors
  ~Enum() noexcept = default;

  [[nodiscard]] auto matches(T variant) const noexcept -> bool {
    return variant == m_variant;
  }
  [[nodiscard]] auto variant() const noexcept -> T { return m_variant; }

  template <T VariantEnum>
  requires(enumeration::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto try_get() const noexcept
      -> optional<const typename enumeration::detail::find_variant_t<
          VariantEnum, Variants...>::type*> {
    using type =
        typename enumeration::detail::find_variant_t<VariantEnum,
                                                     Variants...>::type;
    if (m_variant != VariantEnum) {
      return {};
    }
    return optional<const type*>{reinterpret_cast<const type*>(&m_storage)};
  }

  template <T VariantEnum>
  requires(enumeration::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto try_get() noexcept
      -> optional<typename enumeration::detail::find_variant_t<
          VariantEnum, Variants...>::type*> {
    using type =
        typename enumeration::detail::find_variant_t<VariantEnum,
                                                     Variants...>::type;
    if (m_variant != VariantEnum) {
      return {};
    }
    return optional<type*>{reinterpret_cast<type*>(&m_storage)};
  }

  template <T VariantValue>
  requires(enumeration::FatVariant<VariantValue, Variants...>)
      [[nodiscard]] auto get() const noexcept -> const
      typename enumeration::detail::find_variant_t<VariantValue,
                                                   Variants...>::type& {
    using type =
        typename enumeration::detail::find_variant_t<VariantValue,
                                                     Variants...>::type;
    DITTO_VERIFY(m_variant == VariantValue);
    return *reinterpret_cast<const type*>(&m_storage);
  }

  template <T VariantValue>
  requires(enumeration::FatVariant<VariantValue, Variants...>)
      [[nodiscard]] auto get(T variant) noexcept ->
      typename enumeration::detail::find_variant_t<VariantValue,
                                                   Variants...>::type& {
    using type =
        typename enumeration::detail::find_variant_t<VariantValue,
                                                     Variants...>::type;
    DITTO_VERIFY(m_variant == VariantValue);
    return *reinterpret_cast<type*>(&m_storage);
  }

 private:
  constexpr static size_t STORAGE_SIZE =
      std::max({sizeof(typename Variants::type)...});
  constexpr static size_t STORAGE_ALIGNEMENT =
      std::max({alignof(typename Variants::type)...});

  T m_variant;
  std::aligned_storage_t<STORAGE_SIZE, STORAGE_ALIGNEMENT> m_storage;

  static_assert(enumeration::detail::validate_variants<Variants...>(),
                "Each variant must appear only once in the list");
};  // namespace Ditto

}  // namespace Ditto

#endif  // DITTO_ENUM_H_

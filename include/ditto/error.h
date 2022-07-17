#ifndef DITTO_ERROR_H_
#define DITTO_ERROR_H_

#include <algorithm>
#include <type_traits>
#include <vector>

#include "ditto/assert.h"
#include "ditto/optional.h"
#include "ditto/type_traits.h"

namespace Ditto {

template <ScopedEnum auto EnumVariant, typename Variant>
struct ErrorVariant final {
  constexpr static decltype(EnumVariant) enum_variant = EnumVariant;
  using type_variant = Variant;

  template <typename... Args>
  static void construct(void* ptr, Args&&... args) noexcept {
    new (ptr) Variant{std::forward<Args>(args)...};
  }

  static void copyConstruct(void* dest, const void* source) noexcept
      requires(std::is_copy_constructible_v<Variant>) {
    const Variant& sourceVar = *reinterpret_cast<const Variant*>(source);
    new (dest) Variant{sourceVar};
  }

  static void moveConstruct(void* dest, void* source) noexcept
      requires(std::is_move_constructible_v<Variant>) {
    Variant& sourceVar = *reinterpret_cast<Variant*>(source);
    new (dest) Variant{std::move(sourceVar)};
  }

  static void moveConstruct(void* dest, void* source) noexcept
      requires(!std::is_move_constructible_v<Variant> &&
               std::is_copy_constructible_v<Variant>) {
    copyConstruct(dest, source);
  }

  static void copyAssign(void* dest, const void* source) noexcept
      requires(std::is_copy_assignable_v<Variant>) {
    const Variant& sourceVar = *reinterpret_cast<const Variant*>(source);
    Variant& destVar = *reinterpret_cast<Variant*>(dest);
    destVar = sourceVar;
  }

  static void moveAssign(void* dest, void* source) noexcept
      requires(std::is_move_assignable_v<Variant>) {
    Variant& sourceVar = *reinterpret_cast<Variant*>(source);
    Variant& destVar = *reinterpret_cast<Variant*>(source);
    destVar = std::move(sourceVar);
  }

  static void moveAssign(void* dest, void* source) noexcept
      requires(!std::is_move_assignable_v<Variant> &&
               std::is_copy_assignable_v<Variant>) {
    copyAssign(dest, source);
  }

  static void destroy(void* ptr) noexcept
      requires(!std::is_trivially_destructible_v<Variant>) {
    reinterpret_cast<Variant*>(ptr)->~Variant();
  }

  static void destroy(void* ptr) noexcept
      requires(std::is_trivially_destructible_v<Variant>) {}
};

namespace error {
namespace detail {

// Counts the number of times the given variant appears in the variant list.
template <ScopedEnum auto variant, typename FirstVar, typename... Vars>
consteval auto validate_single_variant(size_t count = 0) -> size_t {
  if constexpr (variant == FirstVar::enum_variant) {
    count++;
  }

  if constexpr (sizeof...(Vars) > 0) {
    return validate_single_variant<variant, Vars...>(count);
  }

  return count;
};

template <typename... Variants>
consteval auto validate_variants() -> bool {
  // Each variant must appear 1 time
  return (
      (validate_single_variant<Variants::enum_variant, Variants...>() == 1) &&
      ...);
}

template <ScopedEnum auto VariantEnum, typename... Variants>
struct FindVariant;

template <ScopedEnum auto VariantEnum, typename V, typename... Others>
struct FindVariant<VariantEnum, ErrorVariant<VariantEnum, V>, Others...> final {
  using type = ErrorVariant<VariantEnum, V>;
};

template <ScopedEnum auto VariantEnum, ScopedEnum auto OtherVariantEnum,
          typename V, typename... Others>
struct FindVariant<VariantEnum, ErrorVariant<OtherVariantEnum, V>, Others...>
    final {
  using type = typename FindVariant<VariantEnum, Others...>::type;
};

template <ScopedEnum auto VariantEnum, typename... Variants>
using find_variant_t = typename FindVariant<VariantEnum, Variants...>::type;

template <ScopedEnum auto VariantEnum, typename FirstVariant,
          typename... Variants>
consteval auto is_fat_variant() -> bool {
  if constexpr (VariantEnum == FirstVariant::enum_variant) {
    return true;
  } else if constexpr (sizeof...(Variants) > 0) {
    return is_fat_variant<VariantEnum, Variants...>();
  } else {
    return false;
  }
}

template <ScopedEnum T, typename... Variants>
struct ErrorVariants;

template <ScopedEnum T>
struct ErrorVariants<T> final {
  static void copyConstruct(T variant, void* ptr, const void* src) noexcept {}
  static void moveConstruct(T variant, void* ptr, void* src) noexcept {}
  static void copyAssign(T variant, void* ptr, const void* src) noexcept {}
  static void moveAssign(T variant, void* ptr, void* src) noexcept {}
  static void destroy(T variant, void* ptr) noexcept {}
};

template <ScopedEnum T, typename FirstVariant, typename... Others>
struct ErrorVariants<T, FirstVariant, Others...> final {
  static void copyConstruct(T variant, void* ptr, const void* src) noexcept {
    if (variant == FirstVariant::enum_variant) {
      FirstVariant::copyConstruct(ptr, src);
      return;
    }
    ErrorVariants<T, Others...>::copyConstruct(variant, ptr, src);
  }
  static void moveConstruct(T variant, void* ptr, void* src) noexcept {
    if (variant == FirstVariant::enum_variant) {
      FirstVariant::moveConstruct(ptr, src);
      return;
    }
    ErrorVariants<T, Others...>::moveConstruct(variant, ptr, src);
  }
  static void copyAssign(T variant, void* ptr, const void* src) noexcept {
    if (variant == FirstVariant::enum_variant) {
      FirstVariant::copyAssign(ptr, src);
      return;
    }
    ErrorVariants<T, Others...>::copyAssign(variant, ptr, src);
  }
  static void moveAssign(T variant, void* ptr, void* src) noexcept {
    if (variant == FirstVariant::enum_variant) {
      FirstVariant::moveAssign(ptr, src);
      return;
    }
    ErrorVariants<T, Others...>::moveAssign(variant, ptr, src);
  }
  static void destroy(T variant, void* ptr) noexcept {
    if (variant == FirstVariant::enum_variant) {
      FirstVariant::destroy(ptr);
      return;
    }
    ErrorVariants<T, Others...>::destroy(variant, ptr);
  }
};

}  // namespace detail

template <ScopedEnum auto>
struct InPlace final {
  explicit InPlace() = default;
};

template <auto VariantEnum, typename... Variants>
concept FatVariant = error::detail::is_fat_variant<VariantEnum, Variants...>();

template <auto VariantEnum, typename... Variants>
concept NotFatVariant =
    !error::detail::is_fat_variant<VariantEnum, Variants...>();

}  // namespace error

template <ScopedEnum T, typename... Variants>
class Error final {
 public:
  template <T VariantName>
  requires(error::NotFatVariant<VariantName, Variants...>) explicit Error(
      error::InPlace<VariantName>) noexcept
      : m_variant(VariantName) {}

  template <T VariantName, typename... Args>
  requires(error::FatVariant<VariantName, Variants...>) explicit Error(
      error::InPlace<VariantName>, Args&&... args) noexcept {
    using Variant =
        typename error::detail::find_variant_t<VariantName, Variants...>;
    m_variant = Variant::enum_variant;
    Variant::construct(&m_storage, std::forward<Args>(args)...);
  }

  Error(const Error& other) noexcept
      requires((std::is_copy_constructible_v<typename Variants::type_variant> &&
                ...))
      : m_variant(other.m_variant) {
    error::detail::ErrorVariants<T, Variants...>::copyConstruct(
        m_variant, &m_storage, &other.m_storage);
  }

  Error(const Error& other) noexcept requires(
      !(std::is_copy_constructible_v<typename Variants::type_variant> &&
        ...)) = delete;

  // Move falls back to copy if not supported by the specific type
  Error(Error&& other) noexcept requires(
      ((std::is_move_constructible_v<typename Variants::type_variant> ||
        std::is_copy_constructible_v<typename Variants::type_variant>)&&...))
      : m_variant(other.m_variant) {
    error::detail::ErrorVariants<T, Variants...>::moveConstruct(
        m_variant, &m_storage, &other.m_storage);
  }

  Error(Error&& other) noexcept requires(!(
      ((std::is_move_constructible_v<typename Variants::type_variant> ||
        std::is_copy_constructible_v<typename Variants::type_variant>)&&...))) =
      delete;

  auto operator=(const Error& other) noexcept -> Error& requires(
      (std::is_copy_assignable_v<typename Variants::type_variant> && ...)) {
    if (this != &other) {
      error::detail::ErrorVariants<T, Variants...>::destroy(m_variant,
                                                            &m_storage);
      m_variant = other.m_variant;
      error::detail::ErrorVariants<T, Variants...>::copyAssign(
          m_variant, &m_storage, &other.m_storage);
    }
    return *this;
  }

  auto operator=(const Error& other) noexcept -> Error& requires(
      !(std::is_copy_assignable_v<typename Variants::type_variant> &&
        ...)) = delete;

  auto operator=(Error&& other) noexcept -> Error& requires(
      ((std::is_move_assignable_v<typename Variants::type_variant> ||
        std::is_copy_assignable_v<typename Variants::type_variant>)&&...)) {
    if (this != &other) {
      error::detail::ErrorVariants<T, Variants...>::destroy(m_variant,
                                                            &m_storage);
      m_variant = other.m_variant;
      error::detail::ErrorVariants<T, Variants...>::moveAssign(
          m_variant, &m_storage, &other.m_storage);
    }
    return *this;
  }

  auto operator=(Error&& other) noexcept -> Error& requires(
      !(((std::is_move_assignable_v<typename Variants::type_variant> ||
          std::is_copy_assignable_v<typename Variants::type_variant>)&&...))) =
      delete;

  // For non-trivially-destructible variants
  ~Error() noexcept requires(
      !(std::is_trivially_destructible_v<typename Variants::type_variant> &&
        ...)) {
    error::detail::ErrorVariants<T, Variants...>::destroy(m_variant,
                                                          &m_storage);
  }

  // Destructor for trivially destructible errors
  ~Error() noexcept = default;

  [[nodiscard]] auto matches(T variant) const noexcept -> bool {
    return variant == m_variant;
  }
  [[nodiscard]] auto variant() const noexcept -> T { return m_variant; }

  template <T VariantEnum>
  requires(error::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto try_get() const noexcept
      -> optional<const typename error::detail::find_variant_t<
          VariantEnum, Variants...>::type_variant*> {
    using type =
        typename error::detail::find_variant_t<VariantEnum,
                                               Variants...>::type_variant;
    if (m_variant != VariantEnum) {
      return {};
    }
    return optional<const type*>{reinterpret_cast<const type*>(&m_storage)};
  }

  template <T VariantEnum>
  requires(error::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto try_get() noexcept
      -> optional<typename error::detail::find_variant_t<
          VariantEnum, Variants...>::type_variant*> {
    using type =
        typename error::detail::find_variant_t<VariantEnum,
                                               Variants...>::type_variant;
    if (m_variant != VariantEnum) {
      return {};
    }
    return optional<type*>{reinterpret_cast<type*>(&m_storage)};
  }

  template <T VariantEnum>
  requires(error::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto get() const noexcept -> const
      typename error::detail::find_variant_t<VariantEnum,
                                             Variants...>::type_variant& {
    using type =
        typename error::detail::find_variant_t<VariantEnum,
                                               Variants...>::type_variant;
    DITTO_VERIFY(m_variant == VariantEnum);
    return *reinterpret_cast<const type*>(&m_storage);
  }

  template <T VariantEnum>
  requires(error::FatVariant<VariantEnum, Variants...>)
      [[nodiscard]] auto get(T variant) noexcept ->
      typename error::detail::find_variant_t<VariantEnum,
                                             Variants...>::type_variant& {
    using type =
        typename error::detail::find_variant_t<VariantEnum,
                                               Variants...>::type_variant;
    DITTO_VERIFY(m_variant == VariantEnum);
    return *reinterpret_cast<type*>(&m_storage);
  }

 private:
  constexpr static size_t STORAGE_SIZE =
      std::max({sizeof(typename Variants::type_variant)...});
  constexpr static size_t STORAGE_ALIGNEMENT =
      std::max({alignof(typename Variants::type_variant)...});

  T m_variant;
  std::aligned_storage_t<STORAGE_SIZE, STORAGE_ALIGNEMENT> m_storage;

  static_assert(error::detail::validate_variants<Variants...>(),
                "Each variant must appear only once in the list");
};  // namespace Ditto

}  // namespace Ditto

#endif

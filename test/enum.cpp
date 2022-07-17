#include "ditto/enum.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using Ditto::Enum;
using Ditto::EnumVariant;
using Ditto::enumeration::InPlace;

struct Stats {
  size_t default_constructed{};
  size_t constructed{};
  size_t copy_constructed{};
  size_t move_constructed{};
  size_t copy_assigned{};
  size_t move_assigned{};
  size_t destructed{};
};

Stats g_stats;

void reset_stats() { memset(&g_stats, 0, sizeof(g_stats)); }

class Nosy {
 public:
  Nosy() noexcept { g_stats.default_constructed++; }
  Nosy(int) noexcept { g_stats.constructed++; }
  Nosy(const Nosy&) noexcept { g_stats.copy_constructed++; }
  Nosy(Nosy&&) noexcept { g_stats.move_constructed++; }
  auto operator=(const Nosy&) noexcept -> Nosy& {
    g_stats.copy_assigned++;
    return *this;
  }
  auto operator=(Nosy&&) noexcept -> Nosy& {
    g_stats.move_assigned++;
    return *this;
  }
  ~Nosy() noexcept { g_stats.destructed++; }
};

class NonMoveable {
 public:
  NonMoveable() noexcept = default;
  NonMoveable(const NonMoveable&) noexcept = default;
  NonMoveable(NonMoveable&&) noexcept = delete;
  auto operator=(const NonMoveable&) noexcept -> NonMoveable& = default;
  auto operator=(NonMoveable&&) noexcept -> NonMoveable& = delete;
  ~NonMoveable() noexcept = default;
};

enum class EnumCodes {
  FuckYou,
  WhyDoYouEvenCare,
  LeaveMeTheFuckAlone,
  OhWell,
  NonCopy,
  Unknown
};

using VariantA = EnumVariant<EnumCodes::FuckYou, int>;
using VariantB = EnumVariant<EnumCodes::WhyDoYouEvenCare, const char*>;
using VariantC = EnumVariant<EnumCodes::LeaveMeTheFuckAlone, std::string>;
using VariantD = EnumVariant<EnumCodes::OhWell, Nosy>;
using VariantE = EnumVariant<EnumCodes::NonCopy, NonMoveable>;

using MyEnum =
    Enum<EnumCodes, VariantA, VariantB, VariantC, VariantD, VariantE>;

namespace TestValidateVariants {

using Ditto::enumeration::detail::validate_variants;

static_assert(validate_variants<VariantA, VariantB, VariantC>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantA>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantB>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantC>());

}  // namespace TestValidateVariants

namespace TestFindVariant {

using Ditto::enumeration::detail::find_variant_t;

static_assert(std::is_same_v<find_variant_t<EnumCodes::LeaveMeTheFuckAlone,
                                            VariantA, VariantB, VariantC>,
                             VariantC>);

static_assert(std::is_same_v<find_variant_t<EnumCodes::WhyDoYouEvenCare,
                                            VariantA, VariantB, VariantC>,
                             VariantB>);

static_assert(std::is_same_v<
              find_variant_t<EnumCodes::FuckYou, VariantA, VariantB, VariantC>,
              VariantA>);

}  // namespace TestFindVariant
//
namespace TestIsFatVariant {

using Ditto::enumeration::detail::is_fat_variant;

static_assert(is_fat_variant<EnumCodes::LeaveMeTheFuckAlone, VariantA, VariantB,
                             VariantC>());

static_assert(
    is_fat_variant<EnumCodes::FuckYou, VariantA, VariantB, VariantC>());

static_assert(is_fat_variant<EnumCodes::WhyDoYouEvenCare, VariantA, VariantB,
                             VariantC>());

static_assert(is_fat_variant<EnumCodes::OhWell, VariantA, VariantB, VariantC,
                             VariantD>());

static_assert(!is_fat_variant<EnumCodes::Unknown, VariantA, VariantB, VariantC,
                              VariantD>());

}  // namespace TestIsFatVariant

TEST(EnumTest, Construction) {
  MyEnum enumeration{InPlace<EnumCodes::FuckYou>{}, 123};

  EXPECT_EQ(enumeration.get<EnumCodes::FuckYou>(), 123);
  EXPECT_TRUE(enumeration.try_get<EnumCodes::FuckYou>().has_value());
  EXPECT_FALSE(
      enumeration.try_get<EnumCodes::LeaveMeTheFuckAlone>().has_value());
}

TEST(EnumTest, NonTrivial) {
  MyEnum enumeration{InPlace<EnumCodes::LeaveMeTheFuckAlone>{}, "Oh boy"};

  EXPECT_EQ(enumeration.get<EnumCodes::LeaveMeTheFuckAlone>(), "Oh boy");
}

TEST(EnumTest, ConstructionAndDestruction) {
  reset_stats();

  {
    MyEnum enumeration{InPlace<EnumCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    static_cast<void>(enumeration.get<EnumCodes::OhWell>());
  }

  EXPECT_EQ(g_stats.default_constructed, 1);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 1);
  EXPECT_EQ(g_stats.copy_constructed, 0);
  EXPECT_EQ(g_stats.move_constructed, 0);
  EXPECT_EQ(g_stats.copy_assigned, 0);
  EXPECT_EQ(g_stats.move_assigned, 0);
}

TEST(EnumTest, CopyConstruction) {
  reset_stats();

  {
    MyEnum enumeration{InPlace<EnumCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyEnum copy{enumeration};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 1);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);
  }

  EXPECT_EQ(g_stats.default_constructed, 1);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 2);
  EXPECT_EQ(g_stats.copy_constructed, 1);
  EXPECT_EQ(g_stats.move_constructed, 0);
  EXPECT_EQ(g_stats.copy_assigned, 0);
  EXPECT_EQ(g_stats.move_assigned, 0);
}

TEST(EnumTest, MoveConstruction) {
  reset_stats();

  {
    MyEnum enumeration{InPlace<EnumCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyEnum copy{std::move(enumeration)};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 1);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);
  }

  EXPECT_EQ(g_stats.default_constructed, 1);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 2);
  EXPECT_EQ(g_stats.copy_constructed, 0);
  EXPECT_EQ(g_stats.move_constructed, 1);
  EXPECT_EQ(g_stats.copy_assigned, 0);
  EXPECT_EQ(g_stats.move_assigned, 0);
}

TEST(EnumTest, CopyAssignment) {
  reset_stats();

  {
    MyEnum enumeration{InPlace<EnumCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyEnum other{InPlace<EnumCodes::OhWell>{}};

    other = enumeration;
    EXPECT_EQ(g_stats.default_constructed, 2);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 1);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 1);
    EXPECT_EQ(g_stats.move_assigned, 0);
  }

  EXPECT_EQ(g_stats.default_constructed, 2);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 3);
  EXPECT_EQ(g_stats.copy_constructed, 0);
  EXPECT_EQ(g_stats.move_constructed, 0);
  EXPECT_EQ(g_stats.copy_assigned, 1);
  EXPECT_EQ(g_stats.move_assigned, 0);
}

TEST(EnumTest, MoveAssignment) {
  reset_stats();

  {
    MyEnum enumeration{InPlace<EnumCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyEnum other{InPlace<EnumCodes::OhWell>{}};

    other = std::move(enumeration);
    EXPECT_EQ(g_stats.default_constructed, 2);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 1);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 1);
  }

  EXPECT_EQ(g_stats.default_constructed, 2);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 3);
  EXPECT_EQ(g_stats.copy_constructed, 0);
  EXPECT_EQ(g_stats.move_constructed, 0);
  EXPECT_EQ(g_stats.copy_assigned, 0);
  EXPECT_EQ(g_stats.move_assigned, 1);
}

TEST(EnumTest, HasNoValue) {
  MyEnum enumeration{InPlace<EnumCodes::Unknown>{}};
  EXPECT_TRUE(enumeration.matches(EnumCodes::Unknown));
}

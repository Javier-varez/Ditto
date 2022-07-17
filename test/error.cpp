#include "ditto/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using Ditto::Error;
using Ditto::ErrorVariant;
using Ditto::error::InPlace;

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

enum class ErrorCodes {
  FuckYou,
  WhyDoYouEvenCare,
  LeaveMeTheFuckAlone,
  OhWell,
  NonCopy,
  Unknown
};

using VariantA = ErrorVariant<ErrorCodes::FuckYou, int>;
using VariantB = ErrorVariant<ErrorCodes::WhyDoYouEvenCare, const char*>;
using VariantC = ErrorVariant<ErrorCodes::LeaveMeTheFuckAlone, std::string>;
using VariantD = ErrorVariant<ErrorCodes::OhWell, Nosy>;
using VariantE = ErrorVariant<ErrorCodes::NonCopy, NonMoveable>;

using MyError =
    Error<ErrorCodes, VariantA, VariantB, VariantC, VariantD, VariantE>;

namespace TestValidateVariants {

using Ditto::error::detail::validate_variants;

static_assert(validate_variants<VariantA, VariantB, VariantC>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantA>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantB>());
static_assert(!validate_variants<VariantA, VariantB, VariantC, VariantC>());

}  // namespace TestValidateVariants

namespace TestFindVariant {

using Ditto::error::detail::find_variant_t;

static_assert(std::is_same_v<find_variant_t<ErrorCodes::LeaveMeTheFuckAlone,
                                            VariantA, VariantB, VariantC>,
                             VariantC>);

static_assert(std::is_same_v<find_variant_t<ErrorCodes::WhyDoYouEvenCare,
                                            VariantA, VariantB, VariantC>,
                             VariantB>);

static_assert(std::is_same_v<
              find_variant_t<ErrorCodes::FuckYou, VariantA, VariantB, VariantC>,
              VariantA>);

}  // namespace TestFindVariant
//
namespace TestIsFatVariant {

using Ditto::error::detail::is_fat_variant;

static_assert(is_fat_variant<ErrorCodes::LeaveMeTheFuckAlone, VariantA,
                             VariantB, VariantC>());

static_assert(
    is_fat_variant<ErrorCodes::FuckYou, VariantA, VariantB, VariantC>());

static_assert(is_fat_variant<ErrorCodes::WhyDoYouEvenCare, VariantA, VariantB,
                             VariantC>());

static_assert(is_fat_variant<ErrorCodes::OhWell, VariantA, VariantB, VariantC,
                             VariantD>());

static_assert(!is_fat_variant<ErrorCodes::Unknown, VariantA, VariantB, VariantC,
                              VariantD>());

}  // namespace TestIsFatVariant

TEST(ErrorTest, Construction) {
  MyError error{InPlace<ErrorCodes::FuckYou>{}, 123};

  EXPECT_EQ(error.get<ErrorCodes::FuckYou>(), 123);
  EXPECT_TRUE(error.try_get<ErrorCodes::FuckYou>().has_value());
  EXPECT_FALSE(error.try_get<ErrorCodes::LeaveMeTheFuckAlone>().has_value());
}

TEST(ErrorTest, NonTrivial) {
  MyError error{InPlace<ErrorCodes::LeaveMeTheFuckAlone>{}, "Oh boy"};

  EXPECT_EQ(error.get<ErrorCodes::LeaveMeTheFuckAlone>(), "Oh boy");
}

TEST(ErrorTest, ConstructionAndDestruction) {
  reset_stats();

  {
    MyError error{InPlace<ErrorCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    static_cast<void>(error.get<ErrorCodes::OhWell>());
  }

  EXPECT_EQ(g_stats.default_constructed, 1);
  EXPECT_EQ(g_stats.constructed, 0);
  EXPECT_EQ(g_stats.destructed, 1);
  EXPECT_EQ(g_stats.copy_constructed, 0);
  EXPECT_EQ(g_stats.move_constructed, 0);
  EXPECT_EQ(g_stats.copy_assigned, 0);
  EXPECT_EQ(g_stats.move_assigned, 0);
}

TEST(ErrorTest, CopyConstruction) {
  reset_stats();

  {
    MyError error{InPlace<ErrorCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyError copy{error};
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

TEST(ErrorTest, MoveConstruction) {
  reset_stats();

  {
    MyError error{InPlace<ErrorCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyError copy{std::move(error)};
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

TEST(ErrorTest, CopyAssignment) {
  reset_stats();

  {
    MyError error{InPlace<ErrorCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyError other{InPlace<ErrorCodes::OhWell>{}};

    other = error;
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

TEST(ErrorTest, MoveAssignment) {
  reset_stats();

  {
    MyError error{InPlace<ErrorCodes::OhWell>{}};
    EXPECT_EQ(g_stats.default_constructed, 1);
    EXPECT_EQ(g_stats.constructed, 0);
    EXPECT_EQ(g_stats.destructed, 0);
    EXPECT_EQ(g_stats.copy_constructed, 0);
    EXPECT_EQ(g_stats.move_constructed, 0);
    EXPECT_EQ(g_stats.copy_assigned, 0);
    EXPECT_EQ(g_stats.move_assigned, 0);

    MyError other{InPlace<ErrorCodes::OhWell>{}};

    other = std::move(error);
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

TEST(ErrorTest, HasNoValue) {
  MyError error{InPlace<ErrorCodes::Unknown>{}};
  EXPECT_TRUE(error.matches(ErrorCodes::Unknown));
}

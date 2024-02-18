#include <gtest/gtest.h>

#include "fintamath/literals/constants/E.hpp"
#include "fintamath/literals/constants/Pi.hpp"

using namespace fintamath;

const E c;

TEST(ETests, toStringTest) {
  EXPECT_EQ(c.toString(), "E");
}

TEST(ETests, callTest) {
  EXPECT_EQ(c()->toString(), "2.7182818284590452354");
}

TEST(ETests, getTypeTest) {
  EXPECT_EQ(E::getTypeStatic(), MathObjectType(MathObjectType::E, "E"));
  EXPECT_EQ(E().getType(), MathObjectType(MathObjectType::E, "E"));
}

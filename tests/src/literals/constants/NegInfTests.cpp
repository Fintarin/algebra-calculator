#include <gtest/gtest.h>

#include "fintamath/literals/constants/NegInf.hpp"
#include "fintamath/literals/constants/Pi.hpp"

using namespace fintamath;

const NegInf c;

TEST(NegInfTests, toStringTest) {
  EXPECT_EQ(c.toString(), "-Inf");
}

TEST(NegInfTests, callTest) {
  EXPECT_EQ(c()->toString(), "-Inf");
}

TEST(NegInfTests, getClassTest) {
  EXPECT_EQ(NegInf::getClassStatic(), MathObjectClass("NegInf"));
  EXPECT_EQ(NegInf::getClassStatic().getParent(), IConstant::getClassStatic());
}

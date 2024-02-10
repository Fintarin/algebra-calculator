#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "fintamath/functions/hyperbolic/Asinh.hpp"

#include "fintamath/literals/Variable.hpp"
#include "fintamath/numbers/Complex.hpp"
#include "fintamath/numbers/Rational.hpp"
#include "fintamath/numbers/Real.hpp"

using namespace fintamath;

using F = Asinh;
const F f;

TEST(AsinhTests, toStringTest) {
  EXPECT_EQ(f.toString(), "asinh");
}

TEST(AsinhTests, getArgumentTypesTest) {
  EXPECT_THAT(f.getArgumentTypes(), testing::ElementsAre(INumber::getTypeStatic()));
}

TEST(AsinhTests, getReturnTypeTest) {
  EXPECT_EQ(f.getReturnType(), INumber::getTypeStatic());
}

TEST(AsinhTests, isVariadicTest) {
  EXPECT_FALSE(F::isVariadicStatic());
  EXPECT_FALSE(f.isVariadic());
}

TEST(AsinhTests, isEvaluatableTest) {
  EXPECT_TRUE(F::isEvaluatableStatic());
  EXPECT_TRUE(f.isEvaluatable());
}

TEST(AsinhTests, callTest) {
  EXPECT_EQ(f(Integer(0))->toString(), "0");
  EXPECT_EQ(f(Integer(1))->toString(), "0.88137358701954302523");

  EXPECT_EQ(f(Rational(1, 10))->toString(), "0.099834078899207563327");
  EXPECT_EQ(f(Rational(-1, 5))->toString(), "-0.19869011034924140647");

  EXPECT_EQ(f(Real("0.5"))->toString(), "0.4812118250596034475");

  EXPECT_EQ(f(Complex(1, 1))->toString(), "asinh(1 + I)");

  EXPECT_EQ(f(Variable("a"))->toString(), "asinh(a)");

  EXPECT_THROW(f(), InvalidInputFunctionException);
  EXPECT_THROW(f(Integer(1), Integer(1), Integer(1)), InvalidInputFunctionException);
}

TEST(AsinhTests, exprTest) {
  EXPECT_EQ(asinhExpr(Integer(10))->toString(), "asinh(10)");
}

TEST(AsinhTests, getTypeTest) {
  EXPECT_EQ(F::getTypeStatic(), MathObjectType::Asinh);
  EXPECT_EQ(f.getType(), MathObjectType::Asinh);
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "fintamath/functions/logarithms/Ln.hpp"

#include "fintamath/literals/Variable.hpp"
#include "fintamath/numbers/Integer.hpp"
#include "fintamath/numbers/Rational.hpp"
#include "fintamath/numbers/Real.hpp"

using namespace fintamath;

using F = Ln;
const F f;

TEST(LnTests, toStringTest) {
  EXPECT_EQ(f.toString(), "ln");
}

TEST(LnTests, getArgumentTypesTest) {
  EXPECT_THAT(f.getArgumentTypes(), testing::ElementsAre(INumber::getTypeStatic()));
}

TEST(LnTests, getReturnTypeTest) {
  EXPECT_EQ(f.getReturnType(), INumber::getTypeStatic());
}

TEST(LnTests, isVariadicTest) {
  EXPECT_FALSE(F::isVariadicStatic());
  EXPECT_FALSE(f.isVariadic());
}

TEST(LnTests, isEvaluatableTest) {
  EXPECT_TRUE(F::isEvaluatableStatic());
  EXPECT_TRUE(f.isEvaluatable());
}

TEST(LnTests, callTest) {
  EXPECT_EQ(f(Integer(1))->toString(), "0");
  EXPECT_EQ(f(Integer(10))->toString(), "2.302585092994045684");
  EXPECT_EQ(f(Integer(5))->toString(), "1.6094379124341003746");
  EXPECT_EQ(f(Rational(1, 10))->toString(), "-2.302585092994045684");
  EXPECT_EQ(f(Real("2.71828"))->toString(), "0.99999932734728200316");

  EXPECT_EQ(f(Integer(-10))->toString(), "ln(-10)");

  EXPECT_EQ(f(Variable("a"))->toString(), "ln(a)");

  EXPECT_THROW(f(), InvalidInputFunctionException);
  EXPECT_THROW(f(Integer(1), Integer(1), Integer(1)), InvalidInputFunctionException);
}

TEST(LnTests, exprTest) {
  EXPECT_EQ(lnExpr(Integer(10))->toString(), "ln(10)");
}

TEST(LnTests, getTypeTest) {
  EXPECT_EQ(F::getTypeStatic(), MathObjectType::Ln);
  EXPECT_EQ(f.getType(), MathObjectType::Ln);
}

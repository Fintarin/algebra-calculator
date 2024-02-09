#include "gtest/gtest.h"

#include "fintamath/functions/hyperbolic/Cosh.hpp"

#include "fintamath/functions/arithmetic/Sub.hpp"
#include "fintamath/functions/arithmetic/UnaryPlus.hpp"
#include "fintamath/literals/Variable.hpp"
#include "fintamath/numbers/Complex.hpp"
#include "fintamath/numbers/Rational.hpp"
#include "fintamath/numbers/Real.hpp"

using namespace fintamath;

const Cosh f;

TEST(CoshTests, toStringTest) {
  EXPECT_EQ(f.toString(), "cosh");
}

TEST(CoshTests, getFunctionTypeTest) {
  EXPECT_EQ(f.getFunctionType(), IFunction::Type::Unary);
}

TEST(CoshTests, callTest) {
  EXPECT_EQ(f(Integer(0))->toString(), "1");
  EXPECT_EQ(f(Integer(5))->toString(), "74.209948524787844444");
  EXPECT_EQ(f(Integer(-5))->toString(), "74.209948524787844444");

  EXPECT_EQ(f(Rational(1, 10))->toString(), "1.005004168055803599");

  EXPECT_EQ(f(Real("0.5"))->toString(), "1.1276259652063807852");

  EXPECT_EQ(f(Complex(1, 1))->toString(), "cosh(1 + I)");

  EXPECT_EQ(f(Variable("a"))->toString(), "cosh(a)");

  EXPECT_THROW(f(), InvalidInputFunctionException);
  EXPECT_THROW(f(Integer(1), Integer(1), Integer(1)), InvalidInputFunctionException);
}

TEST(CoshTests, exprTest) {
  EXPECT_EQ(coshExpr(Integer(10))->toString(), "cosh(10)");
}

TEST(CoshTests, getTypeTest) {
  EXPECT_EQ(Cosh::getTypeStatic(), MathObjectType::Cosh);
  EXPECT_EQ(Cosh().getType(), MathObjectType::Cosh);
}

#pragma once

#include "fintamath/core/MathObjectClass.hpp"
#include "fintamath/expressions/interfaces/IUnaryExpression.hpp"
#include "fintamath/functions/FunctionArguments.hpp"
#include "fintamath/functions/IFunction.hpp"

namespace fintamath {

class SignExpr final : public IUnaryExpressionCRTP<SignExpr> {
  FINTAMATH_CLASS_BODY(SignExpr)

public:
  explicit SignExpr(ArgumentPtr inChild);

protected:
  SimplifyFunctionVector getFunctionsForPostSimplify() const override;

private:
  static ArgumentPtr constSimplify(const IFunction &func, const ArgumentPtr &rhs);

  static ArgumentPtr intApproximateSimplify(const IFunction &func, const ArgumentPtr &rhs);

  static ArgumentPtr negSimplify(const IFunction &func, const ArgumentPtr &rhs);
};

}

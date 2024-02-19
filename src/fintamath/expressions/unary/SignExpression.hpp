#pragma once

#include "fintamath/core/MathObjectType.hpp"
#include "fintamath/expressions/interfaces/IUnaryExpression.hpp"
#include "fintamath/functions/FunctionArguments.hpp"
#include "fintamath/functions/IFunction.hpp"

namespace fintamath {

class SignExpression final : public IUnaryExpressionCRTP<SignExpression> {
public:
  explicit SignExpression(ArgumentPtr inChild);

  static constexpr MathObjectType getTypeStatic() {
    return {MathObjectType::SignExpression, "SignExpression"};
  }

protected:
  SimplifyFunctionVector getFunctionsForPostSimplify() const override;

private:
  static ArgumentPtr constSimplify(const IFunction &func, const ArgumentPtr &rhs);

  static ArgumentPtr intApproximateSimplify(const IFunction &func, const ArgumentPtr &rhs);

  static ArgumentPtr negSimplify(const IFunction &func, const ArgumentPtr &rhs);
};

}

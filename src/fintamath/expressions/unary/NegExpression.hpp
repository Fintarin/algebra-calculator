#pragma once

#include "fintamath/expressions/unary/IUnaryExpression.hpp"

namespace fintamath {

class NegExpression : public IUnaryExpressionCRTP<NegExpression> {
public:
  explicit NegExpression(const std::shared_ptr<IMathObject> &child);

protected:
  std::shared_ptr<IMathObject> preSimplify() const override;

  std::shared_ptr<IMathObject> postSimplify() const override;
};

}
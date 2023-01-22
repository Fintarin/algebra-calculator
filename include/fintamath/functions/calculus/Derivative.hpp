#pragma once

#include "fintamath/functions/IOperator.hpp"

namespace fintamath {

class Derivative : public IOperatorCRTP<Derivative, IMathObject> { // TODO: use IExpression instead
public:
  Derivative() : IOperatorCRTP(IOperator::Priority::PostfixUnary) {
  }

  std::string toString() const override {
    return "'";
  }

protected:
  MathObjectPtr call(const ArgumentsVector &argsVect) const override;
};

}
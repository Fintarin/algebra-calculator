#pragma once

#include "fintamath/functions/IOperator.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

class Angle : public IOperatorCRTP<INumber, Angle, INumber> {
public:
  Angle() : IOperatorCRTP(IOperator::Priority::PostfixUnary) {
  }

  string toString() const override {
    return "°";
  }

protected:
  unique_ptr<IMathObject> call(const ArgumentsRefVector &argsVect) const override;
};

}

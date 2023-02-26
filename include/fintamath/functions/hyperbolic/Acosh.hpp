#pragma once

#include "fintamath/functions/IFunction.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

class Acosh : public IFunctionCRTP<INumber, Acosh, INumber> {
public:
  Acosh() = default;

  std::string toString() const override {
    return "acosh";
  }

protected:
  MathObjectPtr call(const ArgumentsVector &argsVect) const override;
};

}
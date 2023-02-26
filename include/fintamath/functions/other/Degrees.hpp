#pragma once

#include "fintamath/functions/IFunction.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

class Degrees : public IFunctionCRTP<INumber, Degrees, INumber> {
public:
  Degrees() = default;

  std::string toString() const override {
    return "degrees";
  }

protected:
  MathObjectPtr call(const ArgumentsVector &argsVect) const override;
};

}
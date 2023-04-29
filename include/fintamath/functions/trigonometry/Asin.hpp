#pragma once

#include "fintamath/functions/IFunction.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

class Asin : public IFunctionCRTP<INumber, Asin, INumber> {
public:
  Asin() = default;

  std::string toString() const override {
    return "asin";
  }

protected:
  std::unique_ptr<IMathObject> call(const ArgumentsRefVector &argsVect) const override;
};

}

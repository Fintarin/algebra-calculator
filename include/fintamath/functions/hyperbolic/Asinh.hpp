#pragma once

#include <memory>
#include <string>

#include "fintamath/core/IMathObject.hpp"
#include "fintamath/core/MathObjectClass.hpp"
#include "fintamath/functions/FunctionArguments.hpp"
#include "fintamath/functions/FunctionUtils.hpp"
#include "fintamath/functions/IFunction.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

class Asinh final : public IFunctionCRTP<INumber, Asinh, INumber> {
  FINTAMATH_CLASS_BODY(Asinh)

public:
  std::string toString() const override {
    return "asinh";
  }

protected:
  std::unique_ptr<IMathObject> call(const ArgumentRefVector &argVect) const override;

private:
  static std::unique_ptr<IMathObject> multiAsinhSimplify(const INumber &rhs);
};

FINTAMATH_FUNCTION_EXPRESSION(Asinh, asinhExpr);

}

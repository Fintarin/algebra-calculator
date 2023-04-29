#include "fintamath/functions/trigonometry/Acot.hpp"

#include "fintamath/numbers/RealFunctions.hpp"

namespace fintamath {

std::unique_ptr<IMathObject> Acot::call(const ArgumentsRefVector &argsVect) const {
  return acot(convert<Real>(argsVect.front())).toMinimalObject();
}

}

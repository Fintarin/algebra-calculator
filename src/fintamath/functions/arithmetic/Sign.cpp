#include "fintamath/functions/arithmetic/Sign.hpp"

#include "fintamath/numbers/Integer.hpp"
#include "fintamath/numbers/Real.hpp"

namespace fintamath {

MathObjectPtr Sign::call(const ArgumentsVector &argsVect) const {
  return std::make_unique<Integer>(convert<Real>(argsVect.front().get()).sign());
}

}
#include "fintamath/functions/powers/Pow.hpp"

#include "fintamath/numbers/NumberFunctions.hpp"

namespace fintamath {

MathObjectPtr Pow::call(const ArgumentsVector &argsVect) const {
  return pow(cast<INumber>(argsVect.front().get()), cast<INumber>(argsVect.back().get()));
}

}

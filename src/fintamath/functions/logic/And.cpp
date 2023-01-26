#include "fintamath/functions/logic/And.hpp"

#include "fintamath/literals/Boolean.hpp"

namespace fintamath {

MathObjectPtr And::call(const ArgumentsVector &argsVect) const {
  return std::make_unique<Boolean>(cast<Boolean>(argsVect.front().get()) && cast<Boolean>(argsVect.back().get()));
}

}

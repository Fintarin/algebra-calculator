#include "fintamath/functions/other/Index.hpp"

namespace fintamath {

MathObjectPtr Index::call(const ArgumentsVector &argsVect) const {
  return makeFunctionExpression(*this, toArgumentsPtrVect(argsVect));
}

}
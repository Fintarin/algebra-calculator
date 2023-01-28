#include "fintamath/functions/other/Factorial.hpp"

#include "fintamath/exceptions/UndefinedUnaryOpearatorException.hpp"
#include "fintamath/numbers/IntegerFunctions.hpp"

namespace fintamath {

MathObjectPtr Factorial::call(const ArgumentsVector &argsVect) const {
  const auto &rhs = argsVect.front().get();

  if (!is<Integer>(rhs)) {
    throw UndefinedUnaryOpearatorException(toString(), rhs.toString(), UndefinedUnaryOpearatorException::Type::Postfix);
  }

  return factorial(cast<Integer>(rhs), order).simplify();
}

}

#include "fintamath/functions/comparison/LessEqv.hpp"

namespace fintamath {

std::unique_ptr<IMathObject> LessEqv::call(const ArgumentsRefVector &argsVect) const {
  const auto &lhs = cast<IComparable>(argsVect.front().get());
  const auto &rhs = cast<IComparable>(argsVect.back().get());

  // TODO! uncomment
  // if (is<Complex>(lhs) || is<Complex>(rhs)) {
  //   return {};
  // }

  return std::make_unique<Boolean>(lhs <= rhs);
}

}

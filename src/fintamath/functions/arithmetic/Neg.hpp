#pragma once

#include "fintamath/functions/Operator.hpp"

namespace fintamath {
  class Neg : public OperatorCRTP<Neg> {
  public:
    Neg() = default;

    std::string toString() const override;

    int getPriority() const override;

    MathObjectPtr operator()(const MathObject &rhs) const;

  protected:
    MathObjectPtr call(const std::vector<std::reference_wrapper<const MathObject>> &argsVect) const override;

  private:
    static constexpr int priority = 1;
  };
}
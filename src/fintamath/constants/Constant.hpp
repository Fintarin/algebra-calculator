#pragma once

#include <memory>

#include "fintamath/numbers/Rational.hpp"

namespace fintamath {
  class Constant : public MathObject {
  public:
    explicit Constant(const std::string &strConst);

    Rational toRational(int64_t precision) const;
    std::string toString() const override;

    std::unique_ptr<MathObject> clone() const override;

    bool equals(const MathObject &rhs) const override;

  private:
    std::string name;
  };

  namespace types {
    bool isConstant(const std::string_view &str);
  }
}

#ifndef FINTAMATH_I_EXPRESSION_CRTP

#include "fintamath/expressions/IExpression.hpp"

namespace fintamath {

template <typename Derived, bool isMultiFunction = false>
class IExpressionCRTP_ : public IExpressionBaseCRTP<Derived, isMultiFunction> {

#endif // FINTAMATH_I_EXPRESSION_CRTP

public:
  bool equals(const Derived &rhs) const override {
    if constexpr (isMultiFunction) {
      if (*this->getFunction() != *rhs.getFunction()) {
        return false;
      }
    }

    ArgumentsPtrVector lhsChildren = this->getChildren();
    ArgumentsPtrVector rhsChildren = rhs.getChildren();

    if (lhsChildren.size() != rhsChildren.size()) {
      return false;
    }

    for (size_t i = 0; i < lhsChildren.size(); i++) {
      if (lhsChildren[i] != rhsChildren[i] && *lhsChildren[i] != *rhsChildren[i]) {
        return false;
      }
    }

    return true;
  }

protected:
  Derived &add(const Derived &rhs) override {
    throw InvalidInputBinaryOperatorException("+", this->toString(), rhs.toString());
  }

  Derived &substract(const Derived &rhs) override {
    throw InvalidInputBinaryOperatorException("-", this->toString(), rhs.toString());
  }

  Derived &multiply(const Derived &rhs) override {
    throw InvalidInputBinaryOperatorException("*", this->toString(), rhs.toString());
  }

  Derived &divide(const Derived &rhs) override {
    throw InvalidInputBinaryOperatorException("/", this->toString(), rhs.toString());
  }

  Derived &negate() override {
    throw InvalidInputUnaryOperatorException("-", this->toString(), InvalidInputUnaryOperatorException::Type::Prefix);
  }

private:
#ifndef FINTAMATH_I_EXPRESSION_CRTP
};
}

#endif // FINTAMATH_I_EXPRESSION_CRTP
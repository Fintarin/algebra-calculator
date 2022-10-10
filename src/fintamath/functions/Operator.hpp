#pragma once

#include "fintamath/functions/Function.hpp"

namespace fintamath {
  class Operator;
  using OperatorPtr = std::unique_ptr<Operator>;

  class Operator : virtual public Function {
  public:
    ~Operator() override = default;
    virtual int getPriority() const = 0;
  };

  template <typename Derived>
  class OperatorCRTP : virtual public Operator, virtual public FunctionCRTP<Derived> {
  public:
    ~OperatorCRTP() override = default;
  };
}
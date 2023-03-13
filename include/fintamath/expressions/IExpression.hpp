#pragma once

#include <memory>
#include <vector>

#include "fintamath/core/IMathObject.hpp"
#include "fintamath/functions/IFunction.hpp"
#include "fintamath/functions/IOperator.hpp"
#include "fintamath/meta/Converter.hpp"
#include "fintamath/parser/Parser.hpp"
#include "fintamath/parser/Tokenizer.hpp"

namespace fintamath {

class IExpression : virtual public IMathObject {
public:
  virtual std::shared_ptr<IFunction> getFunction() const = 0;

  virtual ArgumentsPtrVector getChildren() const = 0;

  // TODO: make this protected
  // virtual void setPrecision(uint8_t precision) = 0;

  // TODO: remove this and implement in Expression
  virtual void validate() const = 0;

  virtual ArgumentsPtrVector getVariables() const {
    return {};
  }

  template <typename T, typename = std::enable_if_t<std::is_base_of_v<IExpression, T>>>
  static void registerType() {
    Parser::registerType<T>(parserVector);
  }

  static std::unique_ptr<IExpression> parse(const std::string &str) {
    return Parser::parse(parserVector, str);
  }

protected:
  // TODO: remove this and implement in Expression
  void validateChildren(const IFunction &func, const ArgumentsPtrVector &args) const;

  static void compressChild(std::shared_ptr<IMathObject> &child);

  static void simplifyChild(std::shared_ptr<IMathObject> &child);

  // static void setMathObjectPrecision(std::shared_ptr<IMathObject> &obj, uint8_t precision);

  virtual std::shared_ptr<IMathObject> simplify() = 0;

private:
  static Parser::Vector<std::unique_ptr<IExpression>, const std::string &> parserVector;
};

template <typename Derived>
class IExpressionCRTP : virtual public IMathObjectCRTP<Derived>, virtual public IExpression {};

}

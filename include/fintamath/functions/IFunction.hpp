#pragma once

#include <algorithm>

#include "fintamath/core/IMathObject.hpp"
#include "fintamath/exceptions/InvalidInputFunctionException.hpp"
#include "fintamath/parser/Parser.hpp"

namespace fintamath {

using ArgumentsVector = std::vector<std::reference_wrapper<const class IMathObject>>;
using ArgumentsPtrVector = std::vector<MathObjectPtr>;
using ArgumentsTypesVector = std::vector<std::reference_wrapper<const std::type_info>>;

class IFunction;
using FunctionPtr = std::unique_ptr<IFunction>;

class IFunction : virtual public IMathObject {
public:
  enum class Type : uint16_t {
    None,    // 0 arguments
    Unary,   // 1 argument
    Binary,  // 2 arguments
    Ternary, // 3 arguments
    Any,     // undefined number of arguments
  };

public:
  virtual IFunction::Type getFunctionType() const = 0;

  virtual const std::type_info &getReturnType() const = 0;

  virtual ArgumentsTypesVector getArgsTypes() const = 0;

  template <typename... Args, typename = std::enable_if_t<(std::is_base_of_v<IMathObject, Args> && ...)>>
  MathObjectPtr operator()(const Args &...args) const {
    ArgumentsVector argsVect = {args...};
    return callAbstract(argsVect);
  }

  MathObjectPtr operator()(const ArgumentsVector &argsVect) const {
    return callAbstract(argsVect);
  }

  template <typename T, typename = std::enable_if_t<std::is_base_of_v<IFunction, T>>>
  static void registerType() {
    Parser::registerType<T>(parserMap);
  }

  static FunctionPtr parse(const std::string &parsedStr, IFunction::Type type = IFunction::Type::Any) {
    Parser::Comparator<const FunctionPtr &> comp = [type](const FunctionPtr &func) {
      return type == IFunction::Type::Any || func->getFunctionType() == type;
    };
    return Parser::parse<FunctionPtr>(parserMap, comp, parsedStr);
  }

  template <typename... Args,
            typename = std::enable_if_t<(std::is_base_of_v<IMathObject, typename Args::element_type> && ...)>>
  static ArgumentsPtrVector buildArgsPtrVect(Args &&...args) {
    ArgumentsPtrVector vect;
    buildArgsPtrVect<0>(vect, std::move(args)...);
    return vect;
  }

  static ArgumentsPtrVector argsVectToArgsPtrVect(const ArgumentsVector &argsVect) {
    ArgumentsPtrVector argsPtrVector;

    for (const auto &arg : argsVect) {
      argsPtrVector.emplace_back(arg.get().clone());
    }

    return argsPtrVector;
  }

protected:
  virtual MathObjectPtr callAbstract(const ArgumentsVector &argsVect) const = 0;

  static const std::function<MathObjectPtr(const IFunction &function, ArgumentsPtrVector &&args)>
      buildFunctionExpression;

private:
  template <size_t i, typename Head, typename... Tail>
  static void buildArgsPtrVect(ArgumentsPtrVector &vect, Head &&head, Tail &&...tail) {
    vect.emplace_back(std::forward<Head>(head));
    buildArgsPtrVect<i + 1>(vect, std::move(tail)...);
  }

  template <size_t i>
  static void buildArgsPtrVect(ArgumentsPtrVector &vect) {
  }

private:
  static Parser::Map<FunctionPtr> parserMap;
};

template <typename Return, typename Derived, typename... Args>
class IFunctionCRTP : virtual public IMathObjectCRTP<Derived>, virtual public IFunction {
public:
  IFunctionCRTP(bool inIsTypeAny = false) : isTypeAny(inIsTypeAny) {
  }

  IFunction::Type getFunctionType() const final {
    return IFunction::Type(sizeof...(Args));
  }

  const std::type_info &getReturnType() const final {
    return typeid(Return);
  }

  ArgumentsTypesVector getArgsTypes() const final {
    ArgumentsTypesVector argsTypes;
    getArgsTypes<0, Args...>(argsTypes);
    return argsTypes;
  }

protected:
  virtual MathObjectPtr call(const ArgumentsVector &argsVect) const = 0;

  MathObjectPtr callAbstract(const ArgumentsVector &argsVect) const final {
    validateArgsSize(argsVect);

    if (!doArgsMatch(argsVect)) {
      return buildFunctionExpression(*this, argsVectToArgsPtrVect(argsVect));
    }

    return call(argsVect);
  }

  virtual bool equals(const Derived &rhs) const {
    return getFunctionType() == rhs.getFunctionType() && toString() == rhs.toString();
  }

private:
  template <size_t i, typename Head, typename... Tail>
  void getArgsTypes(ArgumentsTypesVector &outArgsTypes) const {
    outArgsTypes.emplace_back(typeid(Head));
    getArgsTypes<i + 1, Tail...>(outArgsTypes);
  }

  template <size_t>
  void getArgsTypes(ArgumentsTypesVector &outArgTypes) const {
  }

  bool doArgsMatch(const ArgumentsVector &argsVect) const {
    if (isTypeAny) {
      return doAnyArgsMatch(argsVect);
    }

    return doArgsMatch<0, Args...>(argsVect);
  }

  template <size_t i, typename Head, typename... Tail>
  bool doArgsMatch(const ArgumentsVector &argsVect) const {
    if (!is<Head>(argsVect.at(i))) {
      return false;
    }

    return doArgsMatch<i + 1, Tail...>(argsVect);
  }

  template <size_t>
  bool doArgsMatch(const ArgumentsVector & /*unused*/) const {
    return true;
  }

  bool doAnyArgsMatch(const ArgumentsVector &argsVect) const {
    return std::all_of(argsVect.begin(), argsVect.end(), [](const auto &arg) {
      return (is<Args>(arg) || ...);
    });
  }

  void validateArgsSize(const ArgumentsVector &argsVect) const {
    if (!isTypeAny && argsVect.size() != sizeof...(Args)) {
      throwInvalidInputFunctionException(argsVect);
    }
  }

  void throwInvalidInputFunctionException(const ArgumentsVector &argsVect) const {
    std::vector<std::string> argNamesVect(argsVect.size());

    for (size_t i = 0; i < argNamesVect.size(); i++) {
      argNamesVect.at(i) = argsVect.at(i).get().toString();
    }

    throw InvalidInputFunctionException(toString(), argNamesVect);
  }

  bool isTypeAny;
};

}

#include "fintamath/expressions/FunctionExpression.hpp"

#include <memory>
#include <string>
#include <utility>

#include "fintamath/core/MathObjectUtils.hpp"
#include "fintamath/expressions/ExpressionUtils.hpp"
#include "fintamath/functions/FunctionArguments.hpp"
#include "fintamath/functions/IFunction.hpp"
#include "fintamath/functions/IOperator.hpp"

namespace fintamath {

using namespace detail;

FunctionExpression::FunctionExpression(const IFunction &inFunc, ArgumentPtrVector inChildren)
    : func(cast<IFunction>(inFunc.clone())) {

  for (auto &&child : std::move(inChildren)) {
    children.emplace_back(std::move(child));
  }
}

std::string FunctionExpression::toString() const {
  if (const auto oper = cast<IOperator>(func)) {
    if (oper->getPriority() == IOperator::Priority::PostfixUnary) {
      return postfixUnaryOperatorToString(*oper, children.front());
    }

    if (oper->getPriority() == IOperator::Priority::PrefixUnary) {
      return prefixUnaryOperatorToString(*oper, children.front());
    }

    return binaryOperatorToString(*oper, children.front(), children.back());
  }

  return functionToString(*func, children);
}

const std::shared_ptr<IFunction> &FunctionExpression::getFunction() const {
  return func;
}

const ArgumentPtrVector &FunctionExpression::getChildren() const {
  return children;
}

ArgumentPtr FunctionExpression::preSimplify() const {
  return simplifyRec(false);
}

ArgumentPtr FunctionExpression::postSimplify() const {
  return simplifyRec(true);
}

ArgumentPtr FunctionExpression::simplifyRec(bool isPostSimplify) const {
  auto simpl = cast<FunctionExpression>(clone());
  ArgumentRefVector args;

  for (auto &child : simpl->children) {
    if (isPostSimplify) {
      postSimplifyChild(child);
    }
    else {
      preSimplifyChild(child);
    }

    args.emplace_back(*child);
  }

  if (!func->doArgsMatch(args)) {
    return simpl;
  }

  return callFunction(*simpl->func, argumentRefVectorToArgumentPtrVector(args));
}

void FunctionExpression::setChildren(const ArgumentPtrVector &childVect) {
  if (childVect.size() != func->getArgumentClasses().size()) {
    throw InvalidInputFunctionException(func->toString(), argumentVectorToStringVector(childVect));
  }

  children = childVect;
}

}

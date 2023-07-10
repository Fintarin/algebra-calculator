#include "fintamath/expressions/binary/LogExpression.hpp"

#include "fintamath/exceptions/InvalidInputException.hpp"
#include "fintamath/exceptions/UndefinedException.hpp"
#include "fintamath/expressions/ExpressionUtils.hpp"
#include "fintamath/functions/arithmetic/Div.hpp"
#include "fintamath/functions/arithmetic/Mul.hpp"
#include "fintamath/functions/calculus/Derivative.hpp"
#include "fintamath/functions/logarithms/Lb.hpp"
#include "fintamath/functions/logarithms/Lg.hpp"
#include "fintamath/functions/logarithms/Ln.hpp"
#include "fintamath/functions/logarithms/Log.hpp"
#include "fintamath/functions/powers/Pow.hpp"
#include "fintamath/literals/Variable.hpp"
#include "fintamath/literals/constants/E.hpp"
#include "fintamath/literals/constants/IConstant.hpp"
#include "fintamath/numbers/INumber.hpp"

namespace fintamath {

LogExpression::LogExpression(const ArgumentPtr &inLhsChild, const ArgumentPtr &inRhsChild)
    : IBinaryExpressionCRTP(Log(), inLhsChild, inRhsChild) {
}

std::string LogExpression::toString() const {
  if (*lhsChild == E()) {
    return functionToString(Ln(), {rhsChild});
  }

  return IBinaryExpression::toString();
}

std::shared_ptr<IFunction> LogExpression::getOutputFunction() const {
  if (*lhsChild == E()) {
    return std::make_shared<Ln>();
  }

  return IBinaryExpression::getFunction();
}

ArgumentPtr LogExpression::preciseSimplify() const {
  if (*lhsChild == E()) {
    auto preciseExpr = cast<LogExpression>(clone());
    preciseSimplifyChild(preciseExpr->rhsChild);
    return preciseExpr;
  }

  return IBinaryExpression::preciseSimplify();
}

LogExpression::SimplifyFunctionsVector LogExpression::getFunctionsForSimplify() const {
  static const LogExpression::SimplifyFunctionsVector simplifyFunctions = {
      &LogExpression::numSimplify,   //
      &LogExpression::equalSimplify, //
  };
  return simplifyFunctions;
}

LogExpression::SimplifyFunctionsVector LogExpression::getFunctionsForPostSimplify() const {
  static const LogExpression::SimplifyFunctionsVector simplifyFunctions = {
      &LogExpression::powSimplify, //
  };
  return simplifyFunctions;
}

ArgumentPtr LogExpression::numSimplify(const IFunction & /*func*/, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (*lhs == Integer(1)) {
    throw UndefinedFunctionException(Log().toString(), {lhs->toString(), rhs->toString()});
  }

  if (*lhs == E()) {
    return callFunction(Ln(), {rhs});
  }

  if (Log().doArgsMatch({*lhs, *rhs})) {
    if (*lhs == Integer(2)) {
      return callFunction(Lb(), {rhs});
    }

    if (*lhs == Integer(10)) {
      return callFunction(Lg(), {rhs});
    }

    return callFunction(Log(), {lhs, rhs});
  }

  return {};
}

ArgumentPtr LogExpression::equalSimplify(const IFunction & /*func*/, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (*lhs == *rhs) {
    return std::make_shared<Integer>(1);
  }

  return {};
}

ArgumentPtr LogExpression::powSimplify(const IFunction & /*func*/, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  ArgumentPtr res;

  if (auto rhsExpr = cast<IExpression>(rhs); rhsExpr && is<Pow>(rhsExpr->getFunction())) {
    ArgumentPtr multiplier = rhsExpr->getChildren().back();
    ArgumentPtr logExpr = makeExpr(Log(), lhs, rhsExpr->getChildren().front());
    res = makeExpr(Mul(), multiplier, logExpr);
  }
  else if (auto lhsExpr = cast<IExpression>(lhs); lhsExpr && is<Pow>(lhsExpr->getFunction())) {
    ArgumentPtr multiplier = makeExpr(Div(), std::make_shared<Integer>(1), lhsExpr->getChildren().back());
    ArgumentPtr logExpr = makeExpr(Log(), lhsExpr->getChildren().front(), rhs);
    res = makeExpr(Mul(), multiplier, logExpr);
  }

  return res;
}

}

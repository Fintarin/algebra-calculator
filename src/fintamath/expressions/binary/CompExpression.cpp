#include "fintamath/expressions/binary/CompExpression.hpp"

#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>

#include "fintamath/core/CoreUtils.hpp"
#include "fintamath/expressions/ExpressionUtils.hpp"
#include "fintamath/expressions/IExpression.hpp"
#include "fintamath/expressions/interfaces/IBinaryExpression.hpp"
#include "fintamath/functions/FunctionArguments.hpp"
#include "fintamath/functions/FunctionUtils.hpp"
#include "fintamath/functions/IFunction.hpp"
#include "fintamath/functions/arithmetic/Add.hpp"
#include "fintamath/functions/arithmetic/Div.hpp"
#include "fintamath/functions/arithmetic/Mul.hpp"
#include "fintamath/functions/arithmetic/Neg.hpp"
#include "fintamath/functions/arithmetic/Sub.hpp"
#include "fintamath/functions/comparison/Eqv.hpp"
#include "fintamath/functions/comparison/Less.hpp"
#include "fintamath/functions/comparison/LessEqv.hpp"
#include "fintamath/functions/comparison/More.hpp"
#include "fintamath/functions/comparison/MoreEqv.hpp"
#include "fintamath/functions/comparison/Neqv.hpp"
#include "fintamath/literals/Boolean.hpp"
#include "fintamath/literals/Variable.hpp"
#include "fintamath/literals/constants/ComplexInf.hpp"
#include "fintamath/literals/constants/Inf.hpp"
#include "fintamath/literals/constants/NegInf.hpp"
#include "fintamath/literals/constants/Undefined.hpp"
#include "fintamath/numbers/Integer.hpp"

namespace fintamath {

using namespace detail;

CompExpression::CompExpression(const IOperator &inOper, ArgumentPtr inLhsChild, ArgumentPtr inRhsChild)
    : IBinaryExpressionCRTP(inOper, std::move(inLhsChild), std::move(inRhsChild)) {
}

std::string CompExpression::toString() const {
  if (isSolution) {
    if (const auto lhsExpr = cast<IExpression>(lhsChild);
        lhsExpr &&
        *lhsExpr->getFunction() == Add{}) {

      ArgumentPtrVector sumChildren = lhsExpr->getChildren();
      ArgumentPtr solLhs = sumChildren.front();

      if (is<Variable>(solLhs)) {
        sumChildren.erase(sumChildren.begin());
        ArgumentPtr solRhs = detail::negate(makePolynom(Add{}, std::move(sumChildren)));
        return CompExpression(cast<IOperator>(*func), std::move(solLhs), std::move(solRhs)).toString();
      }
    }
  }

  return IBinaryExpression::toString();
}

ArgumentPtr CompExpression::preSimplify() const {
  auto simpl = IBinaryExpression::preSimplify();

  if (const auto simplExpr = cast<CompExpression>(simpl)) {
    if (!simplExpr->isSolution &&
        (!is<Integer>(simplExpr->rhsChild) || *simplExpr->rhsChild != Integer(0))) {

      if (*func != Eqv{} &&
          *func != Neqv{} &&
          (containsComplex(simplExpr->lhsChild) || containsComplex(simplExpr->rhsChild))) {

        return simpl;
      }

      if (!containsInfinity(simplExpr->lhsChild) && !containsInfinity(simplExpr->rhsChild)) {
        ArgumentPtr resLhs = subExpr(simplExpr->lhsChild, simplExpr->rhsChild);
        preSimplifyChild(resLhs);
        return CompExpression(cast<IOperator>(*func), resLhs, Integer(0).clone()).clone();
      }
    }
  }

  return simpl;
}

CompExpression::SimplifyFunctionVector CompExpression::getFunctionsForPreSimplify() const {
  static const SimplifyFunctionVector simplifyFunctions = {
      &CompExpression::constSimplify,
  };
  return simplifyFunctions;
}

CompExpression::SimplifyFunctionVector CompExpression::getFunctionsForPostSimplify() const {
  static const SimplifyFunctionVector simplifyFunctions = {
      &CompExpression::constSimplify,
      &CompExpression::divSimplify,
      &CompExpression::negSimplify,
      &CompExpression::rateSimplify,
      &CompExpression::approxSimplify,
  };
  return simplifyFunctions;
}

void CompExpression::markAsSolution() {
  isSolution = true;
}

std::shared_ptr<IFunction> CompExpression::getOppositeFunction(const IFunction &function) {
  static const std::unordered_map<std::string, std::shared_ptr<IFunction>> nameToOppositeFuncMap = {
      {Eqv{}.toString(), std::make_shared<Eqv>()},
      {Neqv{}.toString(), std::make_shared<Neqv>()},
      {More{}.toString(), std::make_shared<Less>()},
      {Less{}.toString(), std::make_shared<More>()},
      {MoreEqv{}.toString(), std::make_shared<LessEqv>()},
      {LessEqv{}.toString(), std::make_shared<MoreEqv>()},
  };
  return nameToOppositeFuncMap.at(function.toString());
}

ArgumentPtr CompExpression::constSimplify(const IFunction &func, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (is<Undefined>(lhs) || is<Undefined>(rhs)) {
    // TODO: return LogicUndefined
    return makeExpr(func, Undefined(), Undefined());
  }

  if ((is<ComplexInf>(lhs) && containsInfinity(rhs)) ||
      (is<ComplexInf>(rhs) && containsInfinity(lhs))) {

    return {};
  }

  if (*lhs == *rhs) {
    Boolean res = is<Eqv>(func) || is<LessEqv>(func) || is<MoreEqv>(func);
    return std::move(res).clone();
  }

  if (is<Inf>(lhs) && is<NegInf>(rhs)) {
    Boolean res = is<Neqv>(func) || is<More>(func) || is<MoreEqv>(func);
    return std::move(res).clone();
  }

  if (is<NegInf>(lhs) && is<Inf>(rhs)) {
    Boolean res = is<Neqv>(func) || is<Less>(func) || is<LessEqv>(func);
    return std::move(res).clone();
  }

  if ((isInfinity(lhs) && !containsInfinity(rhs)) ||
      (isInfinity(rhs) && !containsInfinity(lhs))) {

    if (is<Eqv>(func)) {
      return Boolean(false).clone();
    }

    if (is<Neqv>(func)) {
      return Boolean(true).clone();
    }
  }

  return {};
}

ArgumentPtr CompExpression::divSimplify(const IFunction &func, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (const auto lhsExpr = cast<IExpression>(lhs); lhsExpr && is<Div>(lhsExpr->getFunction())) {
    return makeExpr(func, lhsExpr->getChildren().front(), rhs);
  }

  return {};
}

ArgumentPtr CompExpression::negSimplify(const IFunction &func, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (*rhs != Integer(0)) {
    return {};
  }

  if (isNegated(lhs)) {
    ArgumentPtr newLhs = negExpr(lhs);
    return makeExpr(*cast<IFunction>(getOppositeFunction(func)), std::move(newLhs), rhs);
  }

  return {};
}

ArgumentPtr CompExpression::rateSimplify(const IFunction &func, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (*rhs != Integer(0)) {
    return {};
  }

  auto lhsExpr = cast<IExpression>(lhs);
  if (!lhsExpr) {
    return {};
  }

  if (func != Eqv{} &&
      func != Neqv{} &&
      (containsComplex(lhs) || containsComplex(rhs))) {

    return {};
  }

  ArgumentPtrVector children;
  std::shared_ptr<const IExpression> firstChildMulExpr;

  if (is<Add>(lhsExpr->getFunction())) {
    children = lhsExpr->getChildren();
    firstChildMulExpr = cast<IExpression>(lhsExpr->getChildren().front());
  }
  else {
    children.emplace_back(lhsExpr);
    firstChildMulExpr = lhsExpr;
  }

  auto [rate, value] = splitMulExpr(firstChildMulExpr);
  if (*rate == Integer(1) || containsInfinity(rate)) {
    return {};
  }

  for (auto &child : stdv::drop(children, 1)) {
    child = divExpr(child, rate);
  }

  {
    ArgumentPtrVector newChildren = firstChildMulExpr->getChildren();
    newChildren.erase(newChildren.begin());
    children.front() = makePolynom(Mul{}, std::move(newChildren));
  }

  ArgumentPtr newLhs = makePolynom(Add{}, std::move(children));
  simplifyChild(newLhs);

  approximateSimplifyChild(rate);
  if (isNegativeNumber(rate)) {
    return makeExpr(*cast<IFunction>(getOppositeFunction(func)), newLhs, rhs);
  }

  return makeExpr(func, newLhs, rhs);
}

ArgumentPtr CompExpression::approxSimplify(const IFunction &func, const ArgumentPtr &lhs, const ArgumentPtr &rhs) {
  if (*rhs != Integer(0) || containsVariable(lhs)) {
    return {};
  }

  ArgumentPtr approxLhs = lhs;
  approximateSimplifyChild(approxLhs);
  if (!is<INumber>(approxLhs)) {
    return {};
  }

  ArgumentPtr res = func(*approxLhs, *rhs);
  if (is<Boolean>(res)) {
    return res;
  }

  return {};
}

}

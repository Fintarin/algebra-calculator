#include "fintamath/expressions/binary/CompExpression.hpp"

#include "fintamath/expressions/ExpressionUtils.hpp"
#include "fintamath/functions/arithmetic/Add.hpp"
#include "fintamath/functions/arithmetic/Neg.hpp"
#include "fintamath/functions/arithmetic/Sub.hpp"
#include "fintamath/literals/Variable.hpp"
#include "fintamath/numbers/Integer.hpp"
#include "fintamath/numbers/NumberConstants.hpp"

namespace fintamath {

CompExpression::CompExpression(const IOperator &oper, const ArgumentPtr &lhsChild, const ArgumentPtr &rhsChild)
    : IBinaryExpressionCRTP(oper, lhsChild, rhsChild) {
}

string CompExpression::toString() const {
  if (isSolution) {
    if (const auto lhsExpr = cast<IExpression>(lhsChild); lhsExpr && *lhsExpr->getFunction() == Add()) {
      ArgumentsPtrVector lhsChildren = lhsExpr->getChildren();

      if (lhsChildren.size() == 2) {
        ArgumentPtr solLhs = lhsChildren.front();
        ArgumentPtr solRhs = lhsChildren.back();

        if (is<Variable>(solLhs) && is<INumber>(solRhs)) {
          solRhs = makeFunctionExpression(Neg(), {solRhs});
          return CompExpression(cast<IOperator>(*func), solLhs, solRhs).toString();
        }
      }
    }
  }

  return IBinaryExpression::toString();
}

ArgumentPtr CompExpression::preSimplify() const {
  if (auto rhsInt = cast<Integer>(rhsChild); !rhsInt || *rhsInt != ZERO) {
    ArgumentPtr resLhs = makeFunctionExpression(Sub(), {lhsChild, rhsChild});
    return std::make_shared<CompExpression>(cast<IOperator>(*func), resLhs, ZERO.clone());
  }

  return {};
}

void CompExpression::markAsSolution() {
  isSolution = true;
}

// unique_ptr<IMathObject> CompExpression::simplify(bool isPrecise) const {
/*SumExpression addExpr(*leftExpr->clone());
addExpr.addElement(NegExpression(*rightExpr).toMinimalObject());

unique_ptr<IMathObject> simplExpr = addExpr.simplify(isPrecise);

if (is<IComparable>(simplExpr)) {
  return (*oper)(*simplExpr, ZERO);
}

auto res = make_unique<CompExpression>(*this);
res->leftExpr = move(simplExpr);
res->rightExpr = ZERO.clone();
return res;*/
// }

/*void CompExpression::validate() const {
  if (const auto *childExpr = cast<IExpression>(leftExpr)) {
    childExpr->validate();
  }
  if (const auto *childExpr = cast<IExpression>(rightExpr)) {
    childExpr->validate();
  }

  this->validateArgs(*getFunction(), {*leftExpr, *rightExpr});
}*/

// string CompExpression::solve() const {
/*Variable x("x");
auto expr = simplify(false);
if (!is<CompExpression>(expr)) {
  return expr->toString();
}
auto copyExpr = cast<CompExpression>(*expr);
if (!is<Eqv>(copyExpr.oper)) {
  return expr->toString();
}
if (!copyExpr.detectOneVariable(x)) {
  return toString();
}

auto results = copyExpr.solvePowEquation(x);
if (results.empty()) {
  return toString();
}
results = sortResult(results);
string resultStr = x.toString() + " in {";
for (const auto &res : results) {
  resultStr += res->toString();
  resultStr += ",";
}
resultStr.pop_back();
resultStr += "}";
return resultStr;*/
// }

// string CompExpression::solve(uint8_t precision) const {
/*Variable x("x");
auto expr = simplify(false);
if (!is<CompExpression>(expr)) {
  return expr->toString();
}
auto copyExpr = cast<CompExpression>(*expr);
if (!is<Eqv>(copyExpr.oper)) {
  return expr->toString();
}
if (!copyExpr.detectOneVariable(x)) {
  auto e = *this;
  e.setPrecision(precision);
  return e.toString();
}

auto results = copyExpr.solvePowEquation(x);
if (results.empty()) {
  auto e = *this;
  e.setPrecision(precision);
  return e.toString();
}
results = sortResult(results);
string resultStr = x.toString() + " in {";
for (const auto &res : results) {
  resultStr += Expression(*res).toString(precision);
  resultStr += ",";
}
resultStr.pop_back();
resultStr += "}";
return resultStr;*/
// }

/*vector<unique_ptr<IMathObject>> CompExpression::solvePowEquation(const Variable &x) const {
  auto results = solveQuadraticEquation(x.clone());
  return results;
}*/
/*
// TODO: v is unused here
vector<unique_ptr<IMathObject>> CompExpression::solveQuadraticEquation(const unique_ptr<IMathObject> &v)
const { auto copyExpr = *this; SumExpression polynom; polynom.addElement({leftExpr->clone()});

  auto maxPowObj = polynom.getPow();
  if (!maxPowObj) {
    return {};
  }

  const auto *maxPow = cast<Integer>(maxPowObj);
  if (*maxPow > TWO) {
    return {};
  }

  vector<unique_ptr<IMathObject>> coefficients;
  for (int i = 0; i <= *maxPow; i++) {
    coefficients.emplace_back(polynom.getPowCoefficient(Integer(i).clone()));
  }

  vector<unique_ptr<IMathObject>> results;
  if (coefficients.size() == 2) {
    results.emplace_back(Neg()(*div(*coefficients.at(0), *coefficients.at(1)).simplify(false)));
    return results;
  }
  if (coefficients.size() == 3) {
    auto discr = (fintamath::pow(*coefficients.at(1), TWO) - mul(Integer(4), *coefficients.at(0), *coefficients.at(2)))
                     .simplify(false);

    if (const auto *discrPtr = cast<Integer>(discr); discrPtr && *discrPtr < ZERO) {
      return {};
    }

    auto sqrt_D = sqrt(*discr);
    auto minus_B = Neg()(*coefficients.at(1));
    auto two_A = mul(*coefficients.at(2), TWO);

    auto x1 = div(sub(*minus_B, sqrt_D), two_A).simplify(false);
    auto x2 = div(sum(*minus_B, sqrt_D), two_A).simplify(false);

    results.emplace_back(move(x1));
    results.emplace_back(move(x2));

    return results;
  }
  return results;
}

bool CompExpression::detectOneVariable(Variable &v) const {
  if (const auto *var = cast<Variable>(leftExpr)) {
    v = *var;
    return true;
  }
  if (const auto *expr = cast<IExpression>(leftExpr)) {
    auto variables = expr->getVariables();
    if (variables.empty()) {
      return false;
    }
    v = cast<Variable>(*variables.at(0));
    for (const auto &var : variables) {
      if (var->toString() != v.toString()) {
        return false;
      }
    }
    return true;
  }
}

bool CompExpression::sortFunc(const unique_ptr<IMathObject> &lhs, const unique_ptr<IMathObject> &rhs) {
  if (const auto *lhsComp = cast<IComparable>(lhs)) {
    if (const auto *rhsComp = cast<IComparable>(rhs)) {
      return *lhsComp < *rhsComp;
    }
  }

  return false;
}

vector<unique_ptr<IMathObject>> CompExpression::sortResult(vector<unique_ptr<IMathObject>> &result)
{ std::sort(result.begin(), result.end(), sortFunc); vector<unique_ptr<IMathObject>> resultWithoutRepeat; for
(auto &val : result) { if (resultWithoutRepeat.empty()) { resultWithoutRepeat.emplace_back(move(val)); continue;
    }
    if (*val == *resultWithoutRepeat.at(resultWithoutRepeat.size() - 1)) {
      continue;
    }
    resultWithoutRepeat.emplace_back(move(val));
  }
  return resultWithoutRepeat;
}
*/

}

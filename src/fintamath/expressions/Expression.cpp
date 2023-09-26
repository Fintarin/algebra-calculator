#include "fintamath/expressions/Expression.hpp"

#include "fintamath/expressions/ExpressionUtils.hpp"
#include "fintamath/expressions/FunctionExpression.hpp"
#include "fintamath/functions/arithmetic/Add.hpp"
#include "fintamath/functions/arithmetic/Div.hpp"
#include "fintamath/functions/arithmetic/Mul.hpp"
#include "fintamath/functions/arithmetic/Neg.hpp"
#include "fintamath/functions/arithmetic/Sub.hpp"
#include "fintamath/functions/other/Comma.hpp"
#include "fintamath/functions/other/Factorial.hpp"
#include "fintamath/literals/Variable.hpp"
#include "fintamath/literals/constants/IConstant.hpp"
#include "fintamath/numbers/Complex.hpp"
#include "fintamath/numbers/Real.hpp"

namespace fintamath {

struct TermWithPriority {
  std::shared_ptr<Term> term;

  IOperator::Priority priority = IOperator::Priority::Lowest;

public:
  TermWithPriority() = default;

  TermWithPriority(std::shared_ptr<Term> inTerm, IOperator::Priority inPriority)
      : term(std::move(inTerm)),
        priority(inPriority) {
  }
};

Expression::Expression() : child(Integer(0).clone()) {
}

Expression::Expression(const std::string &str) : child(parseExpr(str)) {
}

Expression::Expression(const ArgumentPtr &obj) : child(compress(obj)) {
}

Expression::Expression(const IMathObject &obj) : Expression(obj.clone()) {
}

Expression::Expression(int64_t val) : child(Integer(val).clone()) {
}

std::string Expression::toString() const {
  simplifyMutable();
  return stringCached;
}

Expression Expression::precise(uint8_t precision) const {
  // TODO: rework so that small ints don't convert to reals
  // TODO: move this transfer to the approximation function
  simplifyMutable();
  Expression preciseExpr(preciseSimplify());
  preciseExpr.simplifyMutable();
  preciseRec(preciseExpr.child, precision);
  preciseExpr.updateStringMutable();
  return preciseExpr;
}

const std::shared_ptr<IFunction> &Expression::getFunction() const {
  static const std::shared_ptr<IFunction> func;
  return func;
}

Expression &Expression::add(const Expression &rhs) {
  child = makeExpr(Add(), *child, *rhs.child);
  isSimplified = false;
  return *this;
}

Expression &Expression::substract(const Expression &rhs) {
  child = makeExpr(Sub(), *child, *rhs.child);
  isSimplified = false;
  return *this;
}

Expression &Expression::multiply(const Expression &rhs) {
  child = makeExpr(Mul(), *child, *rhs.child);
  isSimplified = false;
  return *this;
}

Expression &Expression::divide(const Expression &rhs) {
  child = makeExpr(Div(), *child, *rhs.child);
  isSimplified = false;
  return *this;
}

Expression &Expression::negate() {
  child = makeExpr(Neg(), *child);
  isSimplified = false;
  return *this;
}

const ArgumentPtrVector &Expression::getChildren() const {
  simplifyMutable();
  childrenCached.front() = child;
  return childrenCached;
}

void Expression::setChildren(const ArgumentPtrVector &childVect) {
  if (childVect.size() != 1) {
    throw InvalidInputFunctionException("", argumentVectorToStringVector(childVect));
  }

  *this = Expression(childVect.front());
}

void Expression::setVariables(const std::vector<std::pair<Variable, ArgumentPtr>> &varsToVals) {
  IExpression::setVariables(varsToVals);
}

void Expression::setVariable(const Variable &var, const Expression &val) {
  setVariables({{var, val.child}});
}

ArgumentPtr Expression::simplify() const {
  return child;
}

ArgumentPtr Expression::preciseSimplify() const {
  ArgumentPtr preciseChild = child;
  preciseSimplifyChild(preciseChild);
  return preciseChild;
}

void Expression::simplifyMutable() const {
  if (!isSimplified) {
    simplifyChild(child);
    isSimplified = true;
    updateStringMutable();
  }
}

void Expression::updateStringMutable() const {
  stringCached = child->toString();
}

ArgumentPtr parseExpr(const std::string &str) {
  try {
    auto tokens = Tokenizer::tokenize(str);
    auto terms = Expression::tokensToTerms(tokens);
    auto stack = Expression::termsToOperands(terms);
    auto expr = Expression::operandsToExpr(stack);
    return expr;
  }
  catch (const InvalidInputException &) {
    throw InvalidInputException(str);
  }
}

TermVector Expression::tokensToTerms(const TokenVector &tokens) {
  if (tokens.empty()) {
    throw InvalidInputException("");
  }

  TermVector terms(tokens.size());

  for (auto i : std::views::iota(0U, terms.size())) {
    if (auto term = Parser::parse(getTermMakers(), (tokens[i]))) {
      terms[i] = std::move(term);
    }
    else {
      terms[i] = std::make_unique<Term>(tokens[i], ArgumentPtr());
    }
  }

  fixOperatorTypes(terms);
  insertMultiplications(terms);
  collapseFactorials(terms);

  return terms;
}

// Use the shunting yard algorithm
// https://en.m.wikipedia.org/wiki/Shunting_yard_algorithm
OperandStack Expression::termsToOperands(const TermVector &terms) {
  std::stack<ArgumentPtr> outStack;
  std::stack<TermWithPriority> operStack;

  for (const auto &term : terms) {
    if (!term->value) {
      if (term->name == "(") {
        operStack.emplace(term, IOperator::Priority::Lowest);
      }
      else if (term->name == ")") {
        while (!operStack.empty() &&
               operStack.top().term->name != "(") {

          outStack.emplace(operStack.top().term->value);
          operStack.pop();
        }

        if (operStack.empty()) {
          throw InvalidInputException("");
        }

        operStack.pop();
      }
      else {
        throw InvalidInputException("");
      }
    }
    else {
      if (is<IFunction>(term->value)) {
        if (auto oper = cast<IOperator>(term->value)) {
          while (!operStack.empty() &&
                 operStack.top().term->name != "(" &&
                 operStack.top().priority <= oper->getOperatorPriority() &&
                 !isPrefixOperator(oper)) {

            outStack.emplace(operStack.top().term->value);
            operStack.pop();
          }

          operStack.emplace(term, oper->getOperatorPriority());
        }
        else {
          operStack.emplace(term, IOperator::Priority::Highest);
        }
      }
      else {
        outStack.emplace(term->value);
      }
    }
  }

  while (!operStack.empty()) {
    if (operStack.top().term->name == "(") {
      throw InvalidInputException("");
    }

    outStack.emplace(operStack.top().term->value);
    operStack.pop();
  }

  return outStack;
}

ArgumentPtr Expression::operandsToExpr(OperandStack &operands) {
  if (operands.empty()) {
    throw InvalidInputException("");
  }

  ArgumentPtr arg = operands.top();
  operands.pop();

  if (auto func = cast<IFunction>(arg)) {
    ArgumentPtr rhsChild = operandsToExpr(operands);

    if (isBinaryOperator(func)) {
      ArgumentPtr lhsChild = operandsToExpr(operands);
      return makeExpr(*func, {lhsChild, rhsChild});
    }

    ArgumentPtrVector children = unwrapComma(rhsChild);

    if (func->getFunctionType() != IFunction::Type::Any &&
        size_t(func->getFunctionType()) != children.size()) {

      func = IFunction::parse(func->toString(), IFunction::Type(children.size()));

      if (!func) {
        throw InvalidInputException("");
      }
    }

    return makeExpr(*func, std::move(children));
  }

  return arg;
}

ArgumentPtrVector Expression::unwrapComma(const ArgumentPtr &child) {
  if (const auto childExpr = cast<IExpression>(child);
      childExpr &&
      is<Comma>(childExpr->getFunction())) {

    const ArgumentPtr &lhs = childExpr->getChildren().front();
    const ArgumentPtr &rhs = childExpr->getChildren().back();

    ArgumentPtrVector children = unwrapComma(lhs);
    children.push_back(rhs);
    return children;
  }

  return {child};
}

void Expression::insertMultiplications(TermVector &terms) {
  static const ArgumentPtr mul = Mul().clone();

  for (size_t i = 1; i < terms.size(); i++) {
    if (canNextTermBeBinaryOperator(*terms[i - 1]) &&
        canPrevTermBeBinaryOperator(*terms[i])) {

      auto term = std::make_shared<Term>(mul->toString(), mul);
      terms.insert(terms.begin() + ptrdiff_t(i), term);
      i++;
    }
  }
}

void Expression::fixOperatorTypes(TermVector &terms) {
  bool isFixed = true;

  if (auto &term = terms.front();
      is<IOperator>(term->value) &&
      !isPrefixOperator(term->value)) {

    term->value = IOperator::parse(term->name, IOperator::Priority::PrefixUnary);
    isFixed = isFixed && term->value;
  }

  if (auto &term = terms.back();
      is<IOperator>(term->value) &&
      !isPostfixOperator(term->value)) {

    term->value = IOperator::parse(term->name, IOperator::Priority::PostfixUnary);
    isFixed = isFixed && term->value;
  }

  if (!isFixed) {
    throw InvalidInputException("");
  }

  if (terms.size() < 3) {
    return;
  }

  for (auto i : std::views::iota(1U, terms.size() - 1)) {
    auto &term = terms[i];
    const auto &termPrev = terms[i - 1];

    if (is<IOperator>(term->value) &&
        !isPrefixOperator(term->value) &&
        !canNextTermBeBinaryOperator(*termPrev)) {

      term->value = IOperator::parse(term->name, IOperator::Priority::PrefixUnary);
      isFixed = isFixed && term->value;
    }
  }

  // TODO: use reverse(iota(1, terms.size() - 1)) when it is work
  for (size_t i = terms.size() - 2; i > 0; i--) {
    auto &term = terms[i];
    const auto &termNext = terms[i + 1];

    if (is<IOperator>(term->value) &&
        !isPostfixOperator(term->value) &&
        !canPrevTermBeBinaryOperator(*termNext)) {

      term->value = IOperator::parse(term->name, IOperator::Priority::PostfixUnary);
      isFixed = isFixed && term->value;
    }
  }

  if (!isFixed) {
    throw InvalidInputException("");
  }
}

void Expression::collapseFactorials(TermVector &terms) {
  for (size_t i = 1; i + 1 < terms.size(); i++) {
    auto &term = terms[i];
    const auto &termNext = terms[i + 1];

    if (auto factorial = cast<Factorial>(term->value);
        factorial &&
        is<Factorial>(termNext->value)) {

      Factorial newFactorial;
      newFactorial.setOrder(factorial->getOrder() + 1);
      term->value = newFactorial.clone();

      terms.erase(terms.begin() + ptrdiff_t(i) + 1);
      i--;
    }
  }
}

bool Expression::canNextTermBeBinaryOperator(const Term &term) {
  return !(isPrefixOperator(term.value) ||
           isBinaryOperator(term.value) ||
           isNonOperatorFunction(term.value) ||
           term.name == "(" ||
           term.name == ",");
}

bool Expression::canPrevTermBeBinaryOperator(const Term &term) {
  return !(isPostfixOperator(term.value) ||
           isBinaryOperator(term.value) ||
           term.name == ")" ||
           term.name == ",");
}

bool Expression::isBinaryOperator(const ArgumentPtr &val) {
  auto oper = cast<IOperator>(val);
  return oper && oper->getFunctionType() == IFunction::Type::Binary;
}

bool Expression::isPrefixOperator(const ArgumentPtr &val) {
  auto oper = cast<IOperator>(val);
  return oper && oper->getOperatorPriority() == IOperator::Priority::PrefixUnary;
}

bool Expression::isPostfixOperator(const ArgumentPtr &val) {
  auto oper = cast<IOperator>(val);
  return oper && oper->getOperatorPriority() == IOperator::Priority::PostfixUnary;
}

bool Expression::isNonOperatorFunction(const ArgumentPtr &val) {
  return is<IFunction>(val) && !is<IOperator>(val);
}

void Expression::preciseRec(ArgumentPtr &arg, uint8_t precision) {
  if (const auto realArg = cast<Real>(arg)) {
    arg = realArg->precise(precision).clone();
  }
  else if (const auto complexArg = cast<Complex>(arg)) {
    arg = Complex(convert<Real>(complexArg->real())->precise(precision),
                  convert<Real>(complexArg->imag())->precise(precision))
              .clone();
  }
  else if (const auto exprArg = cast<IExpression>(arg)) {
    ArgumentPtrVector newChildren = exprArg->getChildren();

    for (auto &child : newChildren) {
      preciseRec(child, precision);
    }

    auto newExprArg = cast<IExpression>(exprArg->clone());
    newExprArg->setChildren(newChildren);
    arg = ArgumentPtr(std::move(newExprArg));
  }
}

ArgumentPtr Expression::compress(const ArgumentPtr &child) {
  if (const auto expr = cast<Expression>(child)) {
    return expr->child;
  }

  return child;
}

std::unique_ptr<IMathObject> makeExpr(const IFunction &func, ArgumentPtrVector &&args) {
  ArgumentPtrVector compressedArgs = std::move(args);
  std::ranges::transform(compressedArgs, compressedArgs.begin(), &Expression::compress);

  Expression::validateFunctionArgs(func, compressedArgs);

  if (auto expr = Parser::parse(Expression::getExpressionMakers(), func.toString(), std::move(compressedArgs))) {
    return expr;
  }

  return FunctionExpression(func, compressedArgs).clone();
}

std::unique_ptr<IMathObject> makeExpr(const IFunction &func, const ArgumentPtrVector &args) {
  return makeExpr(func, ArgumentPtrVector(args));
}

std::unique_ptr<IMathObject> makeExpr(const IFunction &func, const ArgumentRefVector &args) {
  return makeExpr(func, argumentRefVectorToArgumentPtrVector(args));
}

void Expression::validateFunctionArgs(const IFunction &func, const ArgumentPtrVector &args) {
  IFunction::Type funcType = func.getFunctionType();

  if ((funcType != IFunction::Type::None && args.empty()) ||
      (funcType != IFunction::Type::Any && args.size() < size_t(funcType))) {

    throw InvalidInputFunctionException(func.toString(), argumentVectorToStringVector(args));
  }

  bool doesArgSizeMatch = funcType != IFunction::Type::Any && args.size() == size_t(funcType);

  ArgumentTypeVector expectedArgTypes = func.getArgTypes();
  MathObjectType expectedType = expectedArgTypes.front();

  for (auto i : std::views::iota(0U, args.size())) {
    if (doesArgSizeMatch) {
      expectedType = expectedArgTypes[i];
    }

    const ArgumentPtr &arg = args[i];

    if (!doesArgMatch(expectedType, arg)) {
      throw InvalidInputFunctionException(func.toString(), argumentVectorToStringVector(args));
    }
  }
}

bool Expression::doesArgMatch(const MathObjectType &expectedType, const ArgumentPtr &arg) {
  if (const auto childExpr = cast<IExpression>(arg)) {
    const std::shared_ptr<IFunction> &childFunc = childExpr->getFunction();
    const MathObjectType childType = childFunc->getReturnType();

    if (childType != Variable::getTypeStatic() &&
        !isBaseOf(expectedType, childType) &&
        !isBaseOf(childType, expectedType)) {

      return false;
    }
  }
  else if (const auto childConst = cast<IConstant>(arg)) {
    const MathObjectType childType = childConst->getReturnType();

    if (!isBaseOf(expectedType, childType) && !isBaseOf(childType, expectedType)) {
      return false;
    }
  }
  else {
    MathObjectType childType = arg->getType();

    if (childType != Variable::getTypeStatic() && !isBaseOf(expectedType, childType)) {
      return false;
    }
  }

  return true;
}

Expression operator+(const Variable &lhs, const Variable &rhs) {
  return Expression(addExpr(lhs, rhs));
}

Expression operator+(const Expression &lhs, const Variable &rhs) {
  return Expression(addExpr(lhs, rhs));
}

Expression operator+(const Variable &lhs, const Expression &rhs) {
  return Expression(addExpr(lhs, rhs));
}

Expression operator-(const Variable &lhs, const Variable &rhs) {
  return Expression(subExpr(lhs, rhs));
}

Expression operator-(const Expression &lhs, const Variable &rhs) {
  return Expression(subExpr(lhs, rhs));
}

Expression operator-(const Variable &lhs, const Expression &rhs) {
  return Expression(subExpr(lhs, rhs));
}

Expression operator*(const Variable &lhs, const Variable &rhs) {
  return Expression(mulExpr(lhs, rhs));
}

Expression operator*(const Expression &lhs, const Variable &rhs) {
  return Expression(mulExpr(lhs, rhs));
}

Expression operator*(const Variable &lhs, const Expression &rhs) {
  return Expression(mulExpr(lhs, rhs));
}

Expression operator/(const Variable &lhs, const Variable &rhs) {
  return Expression(divExpr(lhs, rhs));
}

Expression operator/(const Expression &lhs, const Variable &rhs) {
  return Expression(divExpr(lhs, rhs));
}

Expression operator/(const Variable &lhs, const Expression &rhs) {
  return Expression(divExpr(lhs, rhs));
}

}

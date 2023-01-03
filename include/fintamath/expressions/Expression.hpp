#pragma once

#include <memory>
#include <vector>

#include "fintamath/core/IArithmetic.hpp"
#include "fintamath/expressions/IExpression.hpp"

namespace fintamath {

class Expression : public IExpressionCRTP<Expression>, public IArithmeticCRTP<Expression> {
public:
  using Vector = std::vector<MathObjectPtr>;

public:
  Expression();

  Expression(const Expression &rhs) noexcept;

  Expression(Expression &&rhs) noexcept;

  Expression &operator=(const Expression &rhs) noexcept;

  Expression &operator=(Expression &&rhs) noexcept;

  ~Expression() override = default;

  explicit Expression(const TokenVector &tokens);

  explicit Expression(const std::string &str);

  explicit Expression(const MathObjectPtr &obj);

  Expression(const IMathObject &obj);

  Expression(int64_t val);

  std::string toString() const override;

  std::string toString(uint8_t precision) const;

  MathObjectPtr simplify() const override;

  std::string solve() const;

  std::string solve(uint8_t precision) const;

  // TODO: implement iterator & remove this
  MathObjectPtr &getInfo();

  // TODO: implement iterator & remove this
  const MathObjectPtr &getInfo() const;

  // TODO: implement iterator & remove this
  Expression::Vector &getChildren();

  // TODO: implement iterator & remove this
  const Expression::Vector &getChildren() const;

  MathObjectPtr compress() const;

  static MathObjectPtr buildFunctionExpression(const IFunction &func, const ArgumentsVector &args);

  uint16_t getBaseOperatorPriority() const override;

  void setPrecision(uint8_t precision) override;

  MathObjectPtr simplify(bool isPrecise) const override;

  std::vector<MathObjectPtr> getVariables() const override;

  void setPrecisionRec(uint8_t precision);

protected:
  Expression &add(const Expression &rhs) override;

  Expression &substract(const Expression &rhs) override;

  Expression &multiply(const Expression &rhs) override;

  Expression &divide(const Expression &rhs) override;

  Expression &negate() override;

private:
  static Expression buildRawFunctionExpression(const IFunction &func, const ArgumentsVector &args);

  static ExpressionPtr buildAddExpression(const IFunction &func, const ArgumentsVector &args);

  static ExpressionPtr buildMulExpression(const IFunction &func, const ArgumentsVector &args);

  static ExpressionPtr buildEqvExpression(const IFunction &func, const ArgumentsVector &args);

  static Vector copy(const Vector &rhs);

  Expression &compressTree();

  void parse(const TokenVector &tokens);

  bool parseNeg(const TokenVector &tokens);

  bool parsePow(const TokenVector &tokens);

  bool parsePercent(const TokenVector &tokens);

  bool parseFactorial(const TokenVector &tokens);

  bool parseFiniteTerm(const TokenVector &tokens);

  bool parseFunction(const TokenVector &tokens);

  bool parseLiteral(const TokenVector &tokens);

  bool parseNumber(const TokenVector &tokens);

  Vector getArgs(const TokenVector &tokens);

  TokenVector splitLiteral(const std::string &token, bool addMultiplyToEnd = false); // TODO: remove this and handle the behavior in Tokenizer

  std::string binaryOperatorToString() const;

  std::string prefixUnaryOperatorToString() const;

  std::string postfixUnaryOperatorToString() const;

  std::string functionToString() const;

  void simplifyFunctionsRec(bool isPrecise);

  void simplifyConstant(bool isPrecise);

  void simplifyPow();

  static Expression simplifyPrefixUnaryOperator(Expression expr);

  static Expression simplifyNeg(Expression expr);

  static Expression simplifyPow(Expression expr);

  MathObjectPtr info;
  Vector children;
};

}

#pragma once

#include "fintamath/core/CoreConstants.hpp"
#include "fintamath/core/IArithmetic.hpp"
#include "fintamath/expressions/IExpression.hpp"
#include "fintamath/literals/Variable.hpp"

namespace fintamath {

struct Term {
  Token name;

  ArgumentPtr value;

public:
  Term() = default;

  Term(std::string inName, ArgumentPtr inValue)
      : name(std::move(inName)),
        value(std::move(inValue)) {
  }
};

using TermVector = std::vector<std::shared_ptr<Term>>;

class Expression : public IExpressionCRTP<Expression> {
public:
  Expression();

  explicit Expression(const std::string &str);

  explicit Expression(const ArgumentPtr &obj);

  Expression(const IMathObject &obj);

  Expression(int64_t val);

  std::string toString() const override;

  Expression precise(uint8_t precision = FINTAMATH_PRECISION) const;

  std::shared_ptr<IFunction> getFunction() const override;

  ArgumentPtrVector getChildren() const override;

  void setChildren(const ArgumentPtrVector &childVect) override;

  void setVariables(const std::vector<std::pair<Variable, ArgumentPtr>> &varsToVals) override;

  void setVariable(const Variable &var, const Expression &val);

  static void registerTermsMaker(Parser::Function<std::unique_ptr<Term>, const Token &> &&maker) {
    Parser::add<Token>(getTermMakers(), std::move(maker));
  }

  template <typename Function, bool isPolynomial = false>
  static void registerFunctionExpressionMaker(auto &&maker);

  static MathObjectType getTypeStatic() {
    return MathObjectType::Expression;
  }

protected:
  Expression &add(const Expression &rhs) override;

  Expression &substract(const Expression &rhs) override;

  Expression &multiply(const Expression &rhs) override;

  Expression &divide(const Expression &rhs) override;

  Expression &negate() override;

  ArgumentPtr simplify() const override;

  ArgumentPtr preciseSimplify() const override;

private:
  void simplifyMutable() const;

  void updateStringMutable() const;

  bool parseOperator(const TermVector &terms, size_t start, size_t end);

  bool parseFunction(const TermVector &terms, size_t start, size_t end);

  bool parseBrackets(const TermVector &terms, size_t start, size_t end);

  bool parseTerm(const TermVector &terms, size_t start, size_t end);

  static ArgumentPtrVector parseFunctionArgs(const TermVector &terms, size_t start, size_t end);

  static TermVector tokensToTerms(const TokenVector &tokens);

  static void insertMultiplications(TermVector &terms);

  static void fixOperatorTypes(TermVector &terms);

  static void collapseFactorials(TermVector &terms);

  static bool canNextTermBeBinaryOperator(const Term &term);

  static bool canPrevTermBeBinaryOperator(const Term &term);

  static bool skipBrackets(const TermVector &terms, size_t &openBracketIndex);

  static void cutBrackets(const TermVector &terms, size_t &start, size_t &end);

  static std::string termsToString(const TermVector &terms);

  static bool isBinaryOperator(const ArgumentPtr &val);

  static bool isPrefixOperator(const ArgumentPtr &val);

  static bool isPostfixOperator(const ArgumentPtr &val);

  static bool isNonOperatorFunction(const ArgumentPtr &val);

  static void validateFunctionArgs(const IFunction &func, const ArgumentPtrVector &args);

  static bool doesArgMatch(const MathObjectType &expectedType, const ArgumentPtr &arg);

  static void preciseRec(ArgumentPtr &arg, uint8_t precision);

  static ArgumentPtr compress(const ArgumentPtr &child);

  friend std::unique_ptr<IMathObject> makeExpr(const IFunction &func, const ArgumentPtrVector &args);

  friend ArgumentPtr parseExpr(const std::string &str);

  friend ArgumentPtr parseExpr(const TermVector &terms);

  friend ArgumentPtr parseExpr(const TermVector &terms, size_t start, size_t end);

  static Parser::Vector<std::unique_ptr<Term>, const Token &> &getTermMakers();

  static Parser::Map<std::unique_ptr<IMathObject>, const ArgumentPtrVector &> &getExpressionMakers();

private:
  mutable ArgumentPtr child;

  mutable std::string stringCached;

  mutable bool isSimplified = false;
};

ArgumentPtr parseExpr(const std::string &str);

ArgumentPtr parseExpr(const TermVector &terms);

ArgumentPtr parseExpr(const TermVector &terms, size_t start, size_t end);

Expression operator+(const Variable &lhs, const Variable &rhs);

Expression operator+(const Expression &lhs, const Variable &rhs);

Expression operator+(const Variable &lhs, const Expression &rhs);

Expression operator-(const Variable &lhs, const Variable &rhs);

Expression operator-(const Expression &lhs, const Variable &rhs);

Expression operator-(const Variable &lhs, const Expression &rhs);

Expression operator*(const Variable &lhs, const Variable &rhs);

Expression operator*(const Expression &lhs, const Variable &rhs);

Expression operator*(const Variable &lhs, const Expression &rhs);

Expression operator/(const Variable &lhs, const Variable &rhs);

Expression operator/(const Expression &lhs, const Variable &rhs);

Expression operator/(const Variable &lhs, const Expression &rhs);

template <typename Function, bool isPolynomial>
inline void Expression::registerFunctionExpressionMaker(auto &&maker) {
  Parser::Function<std::unique_ptr<IMathObject>, const ArgumentPtrVector &> constructor =
      [maker = std::forward<decltype(maker)>(maker)](const ArgumentPtrVector &args) {
        static const IFunction::Type type = Function().getFunctionType();
        std::unique_ptr<IMathObject> res;

        if constexpr (IsFunctionTypeAny<Function>::value) {
          res = maker(args);
        }
        else if constexpr (isPolynomial) {
          if (size_t(type) <= args.size()) {
            res = maker(args);
          }
        }
        else {
          if (size_t(type) == args.size()) {
            res = maker(args);
          }
        }

        return res;
      };

  Parser::add<Function>(getExpressionMakers(), std::move(constructor));
}

}

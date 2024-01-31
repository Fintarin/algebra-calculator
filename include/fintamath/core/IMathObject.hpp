#pragma once

#include <memory>
#include <sstream>
#include <string>

#include "fintamath/core/Converter.hpp"
#include "fintamath/core/CoreUtils.hpp"
#include "fintamath/core/Parser.hpp"

namespace fintamath {

class IMathObject {
  using MathObjectParser = Parser<std::unique_ptr<IMathObject>>;

public:
  virtual ~IMathObject() = default;

  virtual std::unique_ptr<IMathObject> clone() const & = 0;

  virtual std::unique_ptr<IMathObject> clone() && = 0;

  virtual std::string toString() const {
    return typeid(*this).name();
  }

  virtual std::unique_ptr<IMathObject> toMinimalObject() const {
    return clone();
  }

  virtual MathObjectType getType() const = 0;

  friend inline bool operator==(const IMathObject &lhs, const IMathObject &rhs) {
    return lhs.equalsAbstract(rhs);
  }

  static void registerConstructor(MathObjectParser::Constructor constructor) {
    getParser().registerConstructor(std::move(constructor));
  }

  static std::unique_ptr<IMathObject> parse(const std::string &str) {
    return getParser().parse(str);
  }

  static MathObjectType getTypeStatic() {
    return MathObjectType::IMathObject;
  }

protected:
  virtual bool equalsAbstract(const IMathObject &rhs) const = 0;

private:
  static MathObjectParser &getParser();
};

template <typename Derived>
class IMathObjectCRTP : public IMathObject {
#define I_MATH_OBJECT_CRTP IMathObjectCRTP
#include "fintamath/core/IMathObjectCRTP.hpp"

#undef I_MATH_OBJECT_CRTP
};

template <std::derived_from<IMathObject> Lhs, ConvertibleToAndNotSameAs<Lhs> Rhs>
bool operator==(const Lhs &lhs, const Rhs &rhs) {
  return lhs == Lhs(rhs);
}

inline std::ostream &operator<<(std::ostream &out, const IMathObject &rhs) {
  return out << rhs.toString();
}

}

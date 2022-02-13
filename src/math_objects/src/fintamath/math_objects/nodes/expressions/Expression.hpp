#pragma once

#include "fintamath/math_objects/nodes/Node.hpp"
#include "fintamath/math_objects/relations/Relation.hpp"

namespace fintamath {
  class Expression : public Node {
  public:
    Expression() = default;
    explicit Expression(const std::string_view &str);

    std::string toString() const override;

  private:
    RelationPtr relation;
    Set set;
  };
}

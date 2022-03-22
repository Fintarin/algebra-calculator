#include <gtest/gtest.h>

#include "fintamath/math_objects/relations/operators/arithmetic_operators/binary_arithmetic_operators/BinaryDivide.hpp"

using namespace fintamath;

const BinaryDivide oper;

TEST(BinaryDivideTests, callOperatorTest) {
  EXPECT_EQ(*oper({std::make_shared<Integer>(20), std::make_shared<Integer>(10)}), Integer(2));
  EXPECT_EQ(*oper({std::make_shared<Rational>(2, 3), std::make_shared<Rational>(3, 4)}), Rational(8, 9));
  EXPECT_EQ(*oper({std::make_shared<Integer>(20), std::make_shared<Rational>(3, 4)}), Rational(80, 3));
  EXPECT_EQ(*oper({std::make_shared<Rational>(2, 3), std::make_shared<Integer>(10)}), Rational(1, 15));

  EXPECT_NE(*oper({std::make_shared<Integer>(20), std::make_shared<Integer>(10)}), Integer(3));
  EXPECT_NE(*oper({std::make_shared<Rational>(2, 3), std::make_shared<Rational>(3, 4)}), Rational(1, 9));
  EXPECT_NE(*oper({std::make_shared<Integer>(20), std::make_shared<Rational>(3, 4)}), Rational(81, 3));
  EXPECT_NE(*oper({std::make_shared<Rational>(2, 3), std::make_shared<Integer>(10)}), Rational(143, 15));

  auto s = std::make_shared<Set>();
  EXPECT_THROW(oper({s, s}), std::invalid_argument);
  EXPECT_THROW(oper({s, std::make_shared<Integer>(20)}), std::invalid_argument);
  EXPECT_THROW(oper({s, std::make_shared<Rational>(20)}), std::invalid_argument);
  EXPECT_THROW(oper({std::make_shared<Integer>(20), s}), std::invalid_argument);
  EXPECT_THROW(oper({std::make_shared<Rational>(20), s}), std::invalid_argument);

  EXPECT_THROW(oper(Set{}), std::invalid_argument);
  EXPECT_THROW(oper({std::make_shared<Integer>(20), std::make_shared<Integer>(20), std::make_shared<Integer>(20)}),
               std::invalid_argument);
}

TEST(BinaryDivideTests, intIntCallOperatorTest) {
  EXPECT_EQ(*oper(Integer("437826432987462398746289734"), Integer("923749326428379569328756")), Integer(473));
}

TEST(BinaryDivideTests, ratRatCallOperatorTest) {
  EXPECT_EQ(*oper(Rational(9825742, 532437), Rational(3445466, 100000000)), Rational(491287100000000, 917246790321));
  EXPECT_EQ(*oper(Rational(3, 4), Rational(6, 8)), Integer(1));
}

TEST(BinaryDivideTests, intRatCallOperatorTest) {
  EXPECT_EQ(*oper(Integer(123083500), Rational(90193108, 13094120)), Rational(402917529755000, 22548277));
}

TEST(BinaryDivideTests, ratIntCallOperatorTest) {
  EXPECT_EQ(*oper(Rational(100000, 10000000), Integer(87539235335)), Rational(1, 8753923533500));
}

TEST(BinaryDivideTests, toStringTest) {
  EXPECT_EQ(oper.toString(), "/");
}

TEST(BinaryDivideTests, getPriorityTest) {
  EXPECT_EQ(oper.getPriority(), 2);
}

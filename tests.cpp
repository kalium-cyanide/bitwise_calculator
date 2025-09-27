

#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "bitwise_calculator.hpp"

uint32_t evaluate(std::string expr)
{
    std::stringstream ss;
    ss << expr;

    std::map<String, std::pair<bool, Integer>> variables;
    Token_Stream ts(ss);

    Parser parser(ts, variables);
    return parser.parse();
}

TEST(BitwiseCalculatorTest, SimpleNumbers)
{
    EXPECT_EQ(evaluate("0"), 0U);
    EXPECT_EQ(evaluate("1"), 1U);
    EXPECT_EQ(evaluate("2"), 2U);
    EXPECT_EQ(evaluate("10"), 10U);
    EXPECT_EQ(evaluate("255"), 255U);
    EXPECT_EQ(evaluate("65535"), 65535U);
    EXPECT_EQ(evaluate("1000000"), 1000000U);
    EXPECT_EQ(evaluate("2147483647"), 2147483647U);
    EXPECT_EQ(evaluate("4294967295"), 4294967295U);
    EXPECT_EQ(evaluate("0b0"), 0U);
    EXPECT_EQ(evaluate("0b11111111"), 255U);
    EXPECT_EQ(evaluate("0b1000"), 8U);
    EXPECT_EQ(evaluate("0xFF"), 255U);
    EXPECT_EQ(evaluate("0x0"), 0U);
    EXPECT_EQ(evaluate("0xABC"), 2748U);
}

TEST(BitwiseCalculatorTest, BitwiseNOT)
{
    EXPECT_EQ(evaluate("~0"), ~0U);
    EXPECT_EQ(evaluate("~1"), ~1U);
    EXPECT_EQ(evaluate("~255"), ~255U);
    EXPECT_EQ(evaluate("~~0"), 0U);
    EXPECT_EQ(evaluate("~~1"), 1U);
    EXPECT_EQ(evaluate("~4294967295"), 0U);
    EXPECT_EQ(evaluate("~(~0)"), 0U);
    EXPECT_EQ(evaluate("~(~1)"), 1U);
    EXPECT_EQ(evaluate("~(~255)"), 255U);
    EXPECT_EQ(evaluate("~(~4294967295)"), 4294967295U);
}

TEST(BitwiseCalculatorTest, LogicalNOT)
{
    EXPECT_EQ(evaluate("!0"), 1U);
    EXPECT_EQ(evaluate("!1"), 0U);
    EXPECT_EQ(evaluate("!2"), 0U);
    EXPECT_EQ(evaluate("!255"), 0U);
    EXPECT_EQ(evaluate("!4294967295"), 0U);
    EXPECT_EQ(evaluate("!!0"), 0U);
    EXPECT_EQ(evaluate("!!1"), 1U);
    EXPECT_EQ(evaluate("!!255"), 1U);
    EXPECT_EQ(evaluate("!(!0)"), 0U);
    EXPECT_EQ(evaluate("!(!1)"), 1U);
}

TEST(BitwiseCalculatorTest, BitwiseAND)
{
    EXPECT_EQ(evaluate("15 & 7"), 7U);
    EXPECT_EQ(evaluate("255 & 0"), 0U);
    EXPECT_EQ(evaluate("255 & 255"), 255U);
    EXPECT_EQ(evaluate("12 & 8"), 8U);
    EXPECT_EQ(evaluate("1 & 1"), 1U);
    EXPECT_EQ(evaluate("1 & 0"), 0U);
    EXPECT_EQ(evaluate("0 & 0"), 0U);
    EXPECT_EQ(evaluate("4294967295 & 1"), 1U);
    EXPECT_EQ(evaluate("15 & 15 & 7"), 7U);
    EXPECT_EQ(evaluate("7 & 15 & 3"), 3U);
}

TEST(BitwiseCalculatorTest, BitwiseXOR)
{
    EXPECT_EQ(evaluate("15 ^ 7"), 8U);
    EXPECT_EQ(evaluate("255 ^ 0"), 255U);
    EXPECT_EQ(evaluate("255 ^ 255"), 0U);
    EXPECT_EQ(evaluate("12 ^ 8"), 4U);
    EXPECT_EQ(evaluate("1 ^ 1"), 0U);
    EXPECT_EQ(evaluate("1 ^ 0"), 1U);
    EXPECT_EQ(evaluate("0 ^ 0"), 0U);
    EXPECT_EQ(evaluate("4294967295 ^ 1"), 4294967295 ^ 1);
    EXPECT_EQ(evaluate("15 ^ 15 ^ 7"), 7U);
    EXPECT_EQ(evaluate("7 ^ 15 ^ 3"), (7 ^ 15 ^ 3));
}

TEST(BitwiseCalculatorTest, BitwiseOR)
{
    EXPECT_EQ(evaluate("15 | 7"), 15U);
    EXPECT_EQ(evaluate("255 | 0"), 255U);
    EXPECT_EQ(evaluate("255 | 255"), 255U);
    EXPECT_EQ(evaluate("12 | 8"), 12U);
    EXPECT_EQ(evaluate("1 | 1"), 1U);
    EXPECT_EQ(evaluate("1 | 0"), 1U);
    EXPECT_EQ(evaluate("0 | 0"), 0U);
    EXPECT_EQ(evaluate("4294967295 | 1"), 4294967295U);
    EXPECT_EQ(evaluate("1 | 2 | 4"), 7U);
    EXPECT_EQ(evaluate("8 | 4 | 2 | 1"), 15U);
}

TEST(BitwiseCalculatorTest, OperatorPrecedence)
{
    EXPECT_EQ(evaluate("1 | 2 & 4"), 1U | 2U & 4U);
    EXPECT_EQ(evaluate("1 & 2 ^ 4"), 1U & 2U ^ 4U);
    EXPECT_EQ(evaluate("1 ^ 2 | 4"), 7U);
    EXPECT_EQ(evaluate("!0 & 1"), 1U);
    EXPECT_EQ(evaluate("~0 & 1"), 1U);
    EXPECT_EQ(evaluate("1 | 2 ^ 3 & 4"), 3U);
    EXPECT_EQ(evaluate("15 & 7 | 8"), 15U);
    EXPECT_EQ(evaluate("1 ^ 2 & 3"), 3U);
    EXPECT_EQ(evaluate("7 | 8 ^ 15 & 1"), Integer(7 | 8 ^ 15 & 1));
    EXPECT_EQ(evaluate("~1 & 3"), 2U);
}

TEST(BitwiseCalculatorTest, Parentheses)
{
    EXPECT_EQ(evaluate("(1 | 2) & 4"), 0U);
    EXPECT_EQ(evaluate("1 | (2 & 4)"), 1U);
    EXPECT_EQ(evaluate("(15 & 7) | 8"), 15U);
    EXPECT_EQ(evaluate("15 & (7 | 8)"), 15U);
    EXPECT_EQ(evaluate("!(0)"), 1U);
    EXPECT_EQ(evaluate("!(1)"), 0U);
    EXPECT_EQ(evaluate("~(0)"), ~0U);
    EXPECT_EQ(evaluate("(1 ^ 2) | (4 & 8)"), 3U);
    EXPECT_EQ(evaluate("((1))"), 1U);
    EXPECT_EQ(evaluate("(((~0)))"), ~0U);
}

TEST(BitwiseCalculatorTest, NOTCombinations)
{
    EXPECT_EQ(evaluate("!(~0)"), 0U);
    EXPECT_EQ(evaluate("~(!0)"), ~1U);
    EXPECT_EQ(evaluate("!(~1)"), 0U);
    EXPECT_EQ(evaluate("~(!1)"), ~0U);
    EXPECT_EQ(evaluate("!(!0)"), 0U);
    EXPECT_EQ(evaluate("~(~0)"), 0U);
    EXPECT_EQ(evaluate("!(~(~0))"), 1U);
    EXPECT_EQ(evaluate("~(!(!0))"), ~0U);
    EXPECT_EQ(evaluate("!(~(!0))"), 0U);
    EXPECT_EQ(evaluate("~(!(~0))"), ~0U);
}

TEST(BitwiseCalculatorTest, ComplexExpressions)
{
    EXPECT_EQ(evaluate("1 | 2 ^ 3 & 4 | 5 ^ 6 & 7"), 1 | 2 ^ 3 & 4 | 5 ^ 6 & 7);
    EXPECT_EQ(evaluate("((1 | 2) ^ (3 & 4)) | (5 ^ (6 & 7))"), ((1 | 2) ^ (3 & 4)) | (5 ^ (6 & 7)));
    EXPECT_EQ(evaluate("!0 | ~0"), ~0U);
    EXPECT_EQ(evaluate("!1 & ~0"), 0U);
    EXPECT_EQ(evaluate("15 & 7 ^ 8 | 1"), 15U);
    EXPECT_EQ(evaluate("~(15 & 7) ^ (8 | 1)"), ((~7U) ^ 9U));
    EXPECT_EQ(evaluate("!(15 & 0) | (1 ^ 1)"), 1U);
    EXPECT_EQ(evaluate("~(!0) & 255"), 254U);
    EXPECT_EQ(evaluate("((~0) & 255) | (!0)"), 255U | 1U);
    EXPECT_EQ(evaluate("1 | 2 | 4 | 8 | 16 | 32 | 64 | 128"), 255U);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

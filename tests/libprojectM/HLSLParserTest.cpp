#include <gtest/gtest.h>

#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>

namespace {

bool ParseHLSL(const char* code)
{
    M4::Allocator allocator;
    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);
    return parser.Parse("test.hlsl", code, strlen(code));
}

} // namespace

// Regression test for issue #940: parenthesized constructor with binary operator
// was rejected with "expected ';'" error.
TEST(HLSLParser, ParenthesizedConstructorWithMultiply)
{
    EXPECT_TRUE(ParseHLSL(
        "float scalar = 2.0;\n"
        "float2 var = (float2(1.0, 2.0)) * scalar;\n"
    ));
}

TEST(HLSLParser, ParenthesizedConstructorWithAdd)
{
    EXPECT_TRUE(ParseHLSL(
        "float2 var = (float2(1.0, 2.0)) + float2(3.0, 4.0);\n"
    ));
}

TEST(HLSLParser, DoubleNestedParensWithOperator)
{
    EXPECT_TRUE(ParseHLSL(
        "float2 var = ((float2(1.0, 2.0))) * 2.0;\n"
    ));
}

TEST(HLSLParser, BothOperandsParenthesized)
{
    EXPECT_TRUE(ParseHLSL(
        "float2 var = (float2(1.0, 2.0)) * (float2(3.0, 4.0));\n"
    ));
}

TEST(HLSLParser, ChainedOperatorsAfterParens)
{
    EXPECT_TRUE(ParseHLSL(
        "float a = 1.0; float b = 2.0; float c = 3.0;\n"
        "float x = (a) * b + c;\n"
    ));
}

TEST(HLSLParser, ConstructorWithoutParensStillWorks)
{
    EXPECT_TRUE(ParseHLSL(
        "float2 var = float2(1.0, 2.0) * 2.0;\n"
    ));
}

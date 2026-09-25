#include <gtest/gtest.h>

#include <Engine.h>
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

TEST(HLSLParser, StringToDoubleStopsAfterNumber)
{
    const char* str = "1.5) * x;";
    char* end = nullptr;
    EXPECT_DOUBLE_EQ(M4::String_ToDouble(str, &end), 1.5);
    EXPECT_EQ(end, str + 3);
}

TEST(HLSLParser, StringToDoubleWithExponent)
{
    const char* str = "1e-3;";
    char* end = nullptr;
    EXPECT_DOUBLE_EQ(M4::String_ToDouble(str, &end), 0.001);
    EXPECT_EQ(end, str + 4);
}

TEST(HLSLParser, StringToDoubleAtEndOfInput)
{
    const char* str = ".25";
    char* end = nullptr;
    EXPECT_DOUBLE_EQ(M4::String_ToDouble(str, &end), 0.25);
    EXPECT_EQ(end, str + 3);
}

TEST(HLSLParser, StringToDoubleRejectsIdentifier)
{
    const char* str = "e1 = 2.0;";
    char* end = nullptr;
    M4::String_ToDouble(str, &end);
    EXPECT_EQ(end, str);
}

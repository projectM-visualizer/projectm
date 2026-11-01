#include <gtest/gtest.h>

#include <Engine.h>
#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

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

// Issue #1043: the comma operator in the increment part of a for loop
// was rejected with "expected ')'".
TEST(HLSLParser, ForLoopIncrementWithCommaOperator)
{
    EXPECT_TRUE(ParseHLSL(
        "float4 PS(float4 position : TEXCOORD0) : COLOR0 {\n"
        "  int j = 100;\n"
        "  float sum = 0.0;\n"
        "  for (int i = 0; i < 10; i++, j++) { sum += j; }\n"
        "  return float4(sum, 0, 0, 1);\n"
        "}\n"
    ));
}

TEST(HLSLParser, ForLoopIncrementWithCommaOperatorGeneratesGLSL)
{
    const char* code =
        "float4 PS(float4 position : TEXCOORD0) : COLOR0 {\n"
        "  int j = 100;\n"
        "  int k = 0;\n"
        "  float sum = 0.0;\n"
        "  for (int i = 0; i < 10; i++, j++, k += 2) { sum += j + k; }\n"
        "  return float4(sum, 0, 0, 1);\n"
        "}\n";

    M4::Allocator allocator;
    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);
    ASSERT_TRUE(parser.Parse("test.hlsl", code, strlen(code)));

    M4::GLSLGenerator generator;
    ASSERT_TRUE(generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader,
                                   M4::GLSLGenerator::Version_330, "PS"));
    std::string glsl = generator.GetResult();
    EXPECT_NE(glsl.find("(i++), (j++), (k += 2)) {"), std::string::npos) << glsl;
}

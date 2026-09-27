#include <gtest/gtest.h>

#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

namespace {

class ModuloTest : public testing::Test
{
protected:
    M4::Allocator allocator;
    M4::HLSLTree tree{&allocator};
    M4::HLSLParser parser{&allocator, &tree};

    bool Parse(const char* source)
    {
        return parser.Parse("modulo.hlsl", source, std::strlen(source));
    }

    M4::HLSLBinaryExpression* Result()
    {
        auto* declaration = tree.FindGlobalDeclaration("result");
        if (!declaration || !declaration->assignment ||
            declaration->assignment->nodeType != M4::HLSLNodeType_BinaryExpression)
            return nullptr;
        return static_cast<M4::HLSLBinaryExpression*>(declaration->assignment);
    }

    std::string Generate(M4::GLSLGenerator::Version version)
    {
        M4::GLSLGenerator generator;
        if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader, version, "PS"))
            return {};
        return generator.GetResult();
    }
};

TEST_F(ModuloTest, FloatVectorResultPreservesComponents)
{
    ASSERT_TRUE(Parse("float3 one; float3 two; float3 result = one % two;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->expressionType.baseType, M4::HLSLBaseType_Float3);
}

TEST_F(ModuloTest, MixedScalarAndVectorPromotesToFloatVector)
{
    ASSERT_TRUE(Parse("float3 one; int two; float3 result = one % two;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->expressionType.baseType, M4::HLSLBaseType_Float3);
}

TEST_F(ModuloTest, UnsignedVectorResultPreservesComponents)
{
    ASSERT_TRUE(Parse("uint3 one; uint3 two; uint3 result = one % two;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->expressionType.baseType, M4::HLSLBaseType_Uint3);
}

TEST_F(ModuloTest, RemainderAndMultiplicationAssociateLeftToRight)
{
    ASSERT_TRUE(Parse("float3 one; float3 two; float3 result = one % two * 2;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->binaryOp, M4::HLSLBinaryOp_Mul);
    ASSERT_EQ(Result()->expression1->nodeType, M4::HLSLNodeType_BinaryExpression);
    EXPECT_EQ(static_cast<M4::HLSLBinaryExpression*>(Result()->expression1)->binaryOp,
              M4::HLSLBinaryOp_Mod);
}

TEST_F(ModuloTest, ComparisonBindsMoreTightlyThanEquality)
{
    ASSERT_TRUE(Parse("int a; int b; int c; bool result = a < b == c;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->binaryOp, M4::HLSLBinaryOp_Equal);
    ASSERT_EQ(Result()->expression1->nodeType, M4::HLSLNodeType_BinaryExpression);
    EXPECT_EQ(static_cast<M4::HLSLBinaryExpression*>(Result()->expression1)->binaryOp,
              M4::HLSLBinaryOp_Less);
}

TEST_F(ModuloTest, BitwiseAndBindsMoreTightlyThanXor)
{
    ASSERT_TRUE(Parse("int a; int b; int c; int result = a ^ b & c;"));
    ASSERT_NE(Result(), nullptr);
    EXPECT_EQ(Result()->binaryOp, M4::HLSLBinaryOp_BitXor);
    ASSERT_EQ(Result()->expression2->nodeType, M4::HLSLNodeType_BinaryExpression);
    EXPECT_EQ(static_cast<M4::HLSLBinaryExpression*>(Result()->expression2)->binaryOp,
              M4::HLSLBinaryOp_BitAnd);
}

TEST_F(ModuloTest, FloatRemainderUsesModOnDesktopAndES)
{
    ASSERT_TRUE(Parse("float4 PS(float4 position : TEXCOORD0) : COLOR0 { "
                      "float3 one = position.xyz; float3 two = float3(0.3, 0.4, 0.5); "
                      "return float4(one % two * 2, 1); }"));
    for (auto version : {M4::GLSLGenerator::Version_330, M4::GLSLGenerator::Version_300_ES,
                         M4::GLSLGenerator::Version_120})
    {
        auto glsl = Generate(version);
        ASSERT_FALSE(glsl.empty());
        EXPECT_NE(glsl.find("mod(one, two)"), std::string::npos);
        EXPECT_EQ(glsl.find("int (one)"), std::string::npos);
    }
}

TEST_F(ModuloTest, IntegerVectorRemainderKeepsIntegerOperator)
{
    ASSERT_TRUE(Parse("float4 PS(float4 position : TEXCOORD0) : COLOR0 { "
                      "int3 one = int3(5, 7, 9); int3 two = int3(2, 3, 4); "
                      "return float4(one % two, 1); }"));
    auto glsl = Generate(M4::GLSLGenerator::Version_330);
    ASSERT_FALSE(glsl.empty());
    EXPECT_NE(glsl.find("one % two"), std::string::npos);
    EXPECT_EQ(glsl.find("int (one)"), std::string::npos);
}

TEST_F(ModuloTest, IntegerVectorRemainderUsesVectorFallbackOnLegacyGLSL)
{
    ASSERT_TRUE(Parse("float4 PS(float4 position : TEXCOORD0) : COLOR0 { "
                      "int3 one = int3(5, 7, 9); int3 two = int3(2, 3, 4); "
                      "return float4(one % two, 1); }"));
    auto glsl = Generate(M4::GLSLGenerator::Version_120);
    ASSERT_FALSE(glsl.empty());
    EXPECT_NE(glsl.find("ivec3(mod("), std::string::npos);
    EXPECT_EQ(glsl.find("int (one)"), std::string::npos);
}

TEST_F(ModuloTest, GeneratedRemainderPromotesScalarAndVectorOperands)
{
    struct Case { const char* declarations; const char* operation; bool floating; };
    for (const auto& test : {
             Case{"float one = 0.75; float two = 0.5;", "one % two", true},
             Case{"float3 one = float3(0.75, 0.5, 0.25); int two = 2;", "one % two", true},
             Case{"int one = 2; float3 two = float3(0.75, 0.5, 0.25);", "one % two", true},
             Case{"int3 one = int3(5, 7, 9); int two = 2;", "one % two", false},
             Case{"uint one = 5; uint two = 2;", "one % two", false},
             Case{"uint3 one = uint3(5, 7, 9); uint two = 2;", "one % two", false}})
    {
        SCOPED_TRACE(test.declarations);
        M4::Allocator localAllocator;
        M4::HLSLTree localTree(&localAllocator);
        M4::HLSLParser localParser(&localAllocator, &localTree);
        std::string source = "float4 PS(float4 position : TEXCOORD0) : COLOR0 { ";
        source += test.declarations;
        source += " float3 result = ";
        source += test.operation;
        source += "; return float4(result, 1); }";
        ASSERT_TRUE(localParser.Parse("mixed-modulo.hlsl", source.c_str(), source.size()));
        for (auto version : {M4::GLSLGenerator::Version_330, M4::GLSLGenerator::Version_300_ES})
        {
            M4::GLSLGenerator generator;
            ASSERT_TRUE(generator.Generate(&localTree, M4::GLSLGenerator::Target_FragmentShader,
                                           version, "PS"));
            std::string glsl = generator.GetResult();
            if (test.floating)
                EXPECT_NE(glsl.find("mod("), std::string::npos);
            else
                EXPECT_NE(glsl.find(" % "), std::string::npos);
            EXPECT_EQ(glsl.find("int (one)"), std::string::npos);
        }
    }
}

} // namespace

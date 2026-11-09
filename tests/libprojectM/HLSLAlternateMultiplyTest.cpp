#include <gtest/gtest.h>

#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

namespace {

/**
 * projectM generates GLSL with Flag_AlternateNanPropagation, which replaces every multiplication with a
 * helper function. The helper's return types must match the HLSL types of the products, as GLSL ES has
 * no implicit conversions between int and float.
 */
class AlternateMultiplyTest : public testing::Test
{
protected:
    M4::Allocator allocator;
    M4::HLSLTree tree{&allocator};
    M4::HLSLParser parser{&allocator, &tree};

    std::string Generate(const char* source)
    {
        if (!parser.Parse("multiply.hlsl", source, std::strlen(source)))
        {
            return {};
        }

        M4::GLSLGenerator generator;
        if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader, M4::GLSLGenerator::Version_300_ES, "PS",
                                M4::GLSLGenerator::Options(M4::GLSLGenerator::Flag_AlternateNanPropagation)))
        {
            return {};
        }
        return generator.GetResult();
    }
};

TEST_F(AlternateMultiplyTest, IntTimesIntReturnsInt)
{
    auto glsl = Generate("float4 PS() : COLOR0 { int a = 2; int b = 3; int c = a * b + 1; return float4(c, 0, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("int mult0(int x, int y)"), std::string::npos) << glsl;
    EXPECT_EQ(glsl.find("float mult0(int i_x, int i_y)"), std::string::npos) << glsl;
}

TEST_F(AlternateMultiplyTest, IntVectorOverloadsExist)
{
    auto glsl = Generate("float4 PS() : COLOR0 { int2 a = int2(1, 2); int2 b = a * a; return float4(b, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("ivec2 mult0(ivec2 x, ivec2 y)"), std::string::npos) << glsl;
    EXPECT_NE(glsl.find("ivec3 mult0(ivec3 x, ivec3 y)"), std::string::npos) << glsl;
    EXPECT_NE(glsl.find("ivec4 mult0(ivec4 x, ivec4 y)"), std::string::npos) << glsl;
}

TEST_F(AlternateMultiplyTest, FloatMultiplicationKeepsNanWorkaround)
{
    auto glsl = Generate("float4 PS() : COLOR0 { float a = 2; float b = a * 3.0; return float4(b, 0, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("float mult0(float x, float y) { if (x == 0.0 || y == 0.0) { return 0.0; }"), std::string::npos) << glsl;
}

} // namespace

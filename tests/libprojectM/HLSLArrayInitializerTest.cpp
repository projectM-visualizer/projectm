#include <gtest/gtest.h>

#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

namespace {

/**
 * Array initializers, as used in many preset shaders, e.g.
 * "const float4 samples[5] = { 0, 0, 0, 11.0/3.0, 0, 1, 0, -2.0/3.0, ... };".
 */
class ArrayInitializerTest : public testing::Test
{
protected:
    M4::Allocator allocator;
    M4::HLSLTree tree{&allocator};
    M4::HLSLParser parser{&allocator, &tree};

    std::string Generate(const char* source)
    {
        if (!parser.Parse("array.hlsl", source, std::strlen(source)))
        {
            return {};
        }

        M4::GLSLGenerator generator;
        if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader, M4::GLSLGenerator::Version_330, "PS"))
        {
            return {};
        }
        return generator.GetResult();
    }
};

TEST_F(ArrayInitializerTest, GlobalArrayIsAssignedByName)
{
    auto glsl = Generate("const float4 samples[2] = { 1, 2, 3, 4, 5, 6, 7, 8 };\n"
                         "float4 PS() : COLOR0 { return samples[1]; }");
    ASSERT_FALSE(glsl.empty());

    // The global is initialized in main(). Assigning to "samples[2]" would write out of bounds.
    EXPECT_EQ(glsl.find("samples[2] ="), std::string::npos) << glsl;
    EXPECT_NE(glsl.find("samples = vec4[]("), std::string::npos) << glsl;
}

TEST_F(ArrayInitializerTest, FlatScalarListIsGroupedIntoVectors)
{
    auto glsl = Generate("const float4 samples[2] = { 1, 2, 3, 4, 5, 6, 7, 8 };\n"
                         "float4 PS() : COLOR0 { return samples[1]; }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("vec4[]( vec4(float (1), float (2), float (3), float (4)), vec4(float (5), float (6), float (7), float (8)) )"),
              std::string::npos)
        << glsl;
}

TEST_F(ArrayInitializerTest, MixedScalarsAndVectorsAreGrouped)
{
    auto glsl = Generate("float4 PS() : COLOR0 { float3 a[2] = { float2(1, 2), 3, 4, 5, 6 }; return float4(a[1], 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("vec3[]( vec3(vec2(1, 2), float (3)), vec3(float (4), float (5), float (6)) )"),
              std::string::npos)
        << glsl;
}

TEST_F(ArrayInitializerTest, VectorListIsUnchanged)
{
    auto glsl = Generate("float4 PS() : COLOR0 { float2 a[2] = { float2(1, 2), float2(3, 4) }; return float4(a[0], a[1]); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("vec2[]( vec2(1, 2), vec2(3, 4) )"), std::string::npos) << glsl;
}

TEST_F(ArrayInitializerTest, ScalarArrayIsUnchanged)
{
    auto glsl = Generate("float4 PS() : COLOR0 { float a[3] = { 1, 2, 3 }; return float4(a[0], a[1], a[2], 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("float[]( 1, 2, 3 )"), std::string::npos) << glsl;
}

} // namespace

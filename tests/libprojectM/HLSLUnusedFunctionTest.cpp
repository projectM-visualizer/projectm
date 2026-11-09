#include <gtest/gtest.h>

#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

namespace {

/**
 * HLSL compilers ignore functions that the entry point doesn't use, so some presets contain unused functions
 * that aren't valid, e.g. without a return statement. These must not end up in the generated GLSL.
 */
class UnusedFunctionTest : public testing::Test
{
protected:
    M4::Allocator allocator;
    M4::HLSLTree tree{&allocator};
    M4::HLSLParser parser{&allocator, &tree};

    std::string Generate(const char* source)
    {
        if (!parser.Parse("functions.hlsl", source, std::strlen(source)))
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

TEST_F(UnusedFunctionTest, UnusedFunctionWithoutReturnIsOmitted)
{
    auto glsl = Generate("float2 rsp(float2 uv_in) { }\n"
                         "float4 PS() : COLOR0 { return float4(1, 0, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_EQ(glsl.find("rsp"), std::string::npos) << glsl;
}

TEST_F(UnusedFunctionTest, CalledFunctionsAreKept)
{
    auto glsl = Generate("float inner(float x) { return x * 2; }\n"
                         "float outer(float x) { return inner(x) + 1; }\n"
                         "float unused(float x) { return x; }\n"
                         "float4 PS() : COLOR0 { return float4(outer(0.5), 0, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("float inner("), std::string::npos) << glsl;
    EXPECT_NE(glsl.find("float outer("), std::string::npos) << glsl;
    EXPECT_EQ(glsl.find("float unused("), std::string::npos) << glsl;
}

TEST_F(UnusedFunctionTest, FunctionUsedInGlobalInitializerIsKept)
{
    auto glsl = Generate("float halve(float x) { return x * 0.5; }\n"
                         "static const float value = halve(3);\n"
                         "float4 PS() : COLOR0 { return float4(value, 0, 0, 1); }");
    ASSERT_FALSE(glsl.empty());

    EXPECT_NE(glsl.find("float halve("), std::string::npos) << glsl;
}

} // namespace

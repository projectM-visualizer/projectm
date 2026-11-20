#include <gtest/gtest.h>

#include <GLSLGenerator.h>
#include <HLSLParser.h>
#include <HLSLTree.h>

#include <MilkdropPreset/MilkdropShader.hpp>

#include <cstring>
#include <string>

// HLSL constructs used by Milkdrop presets that the HLSL parser or GLSL generator didn't handle.

namespace {

bool ParseHLSL(const char* code)
{
    M4::Allocator allocator;
    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);
    return parser.Parse("test.hlsl", code, strlen(code));
}

// Parses the code and generates a GLSL fragment shader with "PS" as the entry point.
// Returns an empty string on failure.
std::string GenerateGLSL(const char* code)
{
    M4::Allocator allocator;
    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);
    if (!parser.Parse("test.hlsl", code, strlen(code)))
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

} // namespace

TEST(HLSLMilkdropCompat, AllIntrinsic)
{
    EXPECT_TRUE(ParseHLSL(
        "float2 modi = float2(1.0, 2.0);\n"
        "float f() { if (all(modi)) { return 1.0; } return 0.0; }\n"));
}

TEST(HLSLMilkdropCompat, AllIntrinsicGeneratesBooleanVectorArgument)
{
    // GLSL's all() and any() only accept boolean vectors.
    const std::string glsl = GenerateGLSL(
        "float2 modi;\n"
        "float4 PS() : COLOR { if (all(modi)) { return 1.0; } return 0.0; }\n");
    ASSERT_FALSE(glsl.empty());
    EXPECT_NE(glsl.find("all(bvec2("), std::string::npos) << glsl;
}

TEST(HLSLMilkdropCompat, AnyIntrinsicOnScalarGeneratesBoolCast)
{
    const std::string glsl = GenerateGLSL(
        "float value;\n"
        "float4 PS() : COLOR { if (any(value)) { return 1.0; } return 0.0; }\n");
    ASSERT_FALSE(glsl.empty());
    EXPECT_EQ(glsl.find("any("), std::string::npos) << glsl;
    EXPECT_NE(glsl.find("bool("), std::string::npos) << glsl;
}

TEST(HLSLMilkdropCompat, SampleAsVariableName)
{
    EXPECT_TRUE(ParseHLSL(
        "float3 f(float3 color)\n"
        "{\n"
        "    float3 sample = color;\n"
        "    sample.y = pow(sample.y, 0.45);\n"
        "    return sample * sample;\n"
        "}\n"));
}

TEST(HLSLMilkdropCompat, SampleAsVariableNameIsRenamedInGLSL)
{
    // "sample" is a keyword in newer GLSL versions.
    const std::string glsl = GenerateGLSL(
        "float3 color;\n"
        "float4 PS() : COLOR { float3 sample = color; return float4(sample * sample, 1.0); }\n");
    ASSERT_FALSE(glsl.empty());
    EXPECT_EQ(glsl.find("sample "), std::string::npos) << glsl;
    EXPECT_EQ(glsl.find("sample*"), std::string::npos) << glsl;
}

TEST(HLSLMilkdropCompat, SampleInterpolationModifierStillWorks)
{
    EXPECT_TRUE(ParseHLSL(
        "float4 PS(sample float4 color : COLOR) : COLOR0 { return color; }\n"));
}

TEST(HLSLMilkdropCompat, SwizzleOnParenthesizedConstructor)
{
    EXPECT_TRUE(ParseHLSL(
        "float3 a; float b;\n"
        "float3 f() { return (float3(a.y, a.x, b).xyy * b); }\n"));
}

TEST(HLSLMilkdropCompat, SaturateOfBool)
{
    const std::string glsl = GenerateGLSL(
        "float mask;\n"
        "float4 PS() : COLOR { return float4(saturate(!mask), 0.0, 0.0, 1.0); }\n");
    ASSERT_FALSE(glsl.empty());
    EXPECT_NE(glsl.find("clamp(float ("), std::string::npos) << glsl;
}

// A comment ending with the word "sampler" must not cause the following line to be removed.
TEST(HLSLMilkdropCompat, SamplerWordInCommentKeepsNextLine)
{
    std::string program =
        "uniform sampler2D sampler_noise_hq;\n"
        "//2. slow moving high quality noise sampler\n"
        "     float mfnoise = tex2D(sampler_noise_hq, uv);\n"
        "sampler sampler_pw_main;\n"
        "float4 texsize_pw_main;\n"
        "ret = mfnoise;\n";

    libprojectM::MilkdropPreset::MilkdropShader::RemoveSamplerAndTexSizeDeclarations(program);

    EXPECT_NE(program.find("float mfnoise = tex2D(sampler_noise_hq, uv);"), std::string::npos) << program;
    EXPECT_EQ(program.find("sampler sampler_pw_main;"), std::string::npos) << program;
    EXPECT_EQ(program.find("float4 texsize_pw_main;"), std::string::npos) << program;
    EXPECT_NE(program.find("ret = mfnoise;"), std::string::npos) << program;
}

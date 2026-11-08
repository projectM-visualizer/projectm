#include <gtest/gtest.h>

#include <MilkdropPreset/MilkdropShader.hpp>

#include <string>

using libprojectM::MilkdropPreset::MilkdropShader;

// These tests call the actual MilkdropShader::PreprocessPresetShader() (no GL context required),
// unlike MilkdropShaderCommentParsingTest.cpp, which tests a copy of the logic.

namespace {

auto Preprocess(MilkdropShader::ShaderType type, const std::string& code) -> std::string
{
    std::string program = code;
    MilkdropShader::PreprocessPresetShader(program, type);
    return program;
}

} // namespace

// Regression test: sampler_state blocks were only removed from the comment-stripped search copy,
// not from the actual shader code, so presets using them failed to parse with
// "expected ';' near 'AddressU'". The removal also shifted the shader_body position.
TEST(MilkdropShaderPreprocess, RemovesSamplerStateBlocks)
{
    const std::string code =
        "sampler sampler_grad = sampler_state {\n"
        " AddressU = WRAP;\n"
        " AddressV = WRAP;\n"
        "};\n"
        "\n"
        "sampler sampler_grad2 = sampler_state {\n"
        " AddressU = WRAP;\n"
        " AddressV = WRAP;\n"
        "};\n"
        "\n"
        "shader_body\n"
        "{\n"
        "    ret = tex2D(sampler_grad, uv).xyz + tex2D(sampler_grad2, uv).xyz;\n"
        "}\n";

    const std::string result = Preprocess(MilkdropShader::ShaderType::CompositeShader, code);

    EXPECT_EQ(result.find("sampler_state"), std::string::npos) << result;
    EXPECT_EQ(result.find("AddressU"), std::string::npos) << result;
    EXPECT_EQ(result.find("shader_body"), std::string::npos) << result;
    EXPECT_NE(result.find("void PS("), std::string::npos) << result;
    EXPECT_NE(result.find("ret = tex2D(sampler_grad, uv).xyz + tex2D(sampler_grad2, uv).xyz;"), std::string::npos) << result;
    EXPECT_NE(result.find("_return_value = float4(ret.xyz, 1.0);"), std::string::npos) << result;
}

TEST(MilkdropShaderPreprocess, KeepsCommentedSamplerStateAndRemovesRealOne)
{
    const std::string code =
        "// sampler sampler_a = sampler_state { Filter = POINT; };\n"
        "sampler sampler_b = sampler_state { Filter = LINEAR; };\n"
        "shader_body\n"
        "{\n"
        "    ret = tex2D(sampler_b, uv).xyz;\n"
        "}\n";

    const std::string result = Preprocess(MilkdropShader::ShaderType::WarpShader, code);

    EXPECT_NE(result.find("Filter = POINT"), std::string::npos) << result;
    EXPECT_EQ(result.find("Filter = LINEAR"), std::string::npos) << result;
    EXPECT_EQ(result.find("shader_body"), std::string::npos) << result;
    EXPECT_NE(result.find("void PS("), std::string::npos) << result;
    EXPECT_NE(result.find("_mv_tex_coords.xy = _uv.xy;"), std::string::npos) << result;
    EXPECT_NE(result.find("ret = tex2D(sampler_b, uv).xyz;"), std::string::npos) << result;
}

// A malformed sampler_state without its own assignment must not cause unrelated code to be removed.
TEST(MilkdropShaderPreprocess, MalformedSamplerStateDoesNotRemoveUnrelatedCode)
{
    // The '=', '}' and ';' found around this sampler_state all belong to code before it.
    const std::string code =
        "float b = 2.0;\n"
        "struct S { float v; };\n"
        "sampler sampler_x : sampler_state\n"
        "shader_body\n"
        "{\n"
        "    ret = b;\n"
        "}\n";

    const std::string result = Preprocess(MilkdropShader::ShaderType::CompositeShader, code);

    EXPECT_NE(result.find("float b = 2.0;"), std::string::npos) << result;
    EXPECT_NE(result.find("struct S { float v; };"), std::string::npos) << result;
    EXPECT_NE(result.find("ret = b;"), std::string::npos) << result;
}

TEST(MilkdropShaderPreprocess, WithoutSamplerStateIsUnaffected)
{
    const std::string code =
        "shader_body\n"
        "{\n"
        "    ret = tex2D(sampler_main, uv).xyz;\n"
        "}\n";

    const std::string result = Preprocess(MilkdropShader::ShaderType::CompositeShader, code);

    EXPECT_EQ(result.find("shader_body"), std::string::npos) << result;
    EXPECT_NE(result.find("void PS("), std::string::npos) << result;
    EXPECT_NE(result.find("ret = tex2D(sampler_main, uv).xyz;"), std::string::npos) << result;
}

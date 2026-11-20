#include <gtest/gtest.h>

#include <MilkdropPreset/MilkdropShader.hpp>

#include <set>
#include <string>

using libprojectM::MilkdropPreset::MilkdropShader;

TEST(MilkdropShaderSampler, UnprefixedDeclaration)
{
    EXPECT_EQ(MilkdropShader::FindUnprefixedSamplers("sampler mytexture;\nshader_body { ret = tex2D(mytexture, uv).xyz; }"),
              std::set<std::string>({"mytexture"}));
}

TEST(MilkdropShaderSampler, WrapAndFilterPrefixesAreKept)
{
    // The texture manager extracts the wrap and filter mode from the name, as for "sampler_pw_noise".
    EXPECT_EQ(MilkdropShader::FindUnprefixedSamplers("sampler pw_noise;"), std::set<std::string>({"pw_noise"}));
}

TEST(MilkdropShaderSampler, TypedAndSpacedDeclarations)
{
    EXPECT_EQ(MilkdropShader::FindUnprefixedSamplers("sampler2D  first ;\nsampler3D\tsecond;\nsampler\nthird;"),
              std::set<std::string>({"first", "second", "third"}));
}

TEST(MilkdropShaderSampler, PrefixedDeclarationsAreIgnored)
{
    EXPECT_TRUE(MilkdropShader::FindUnprefixedSamplers("sampler sampler_noise_lq;\nsampler2D sampler_main;").empty());
}

TEST(MilkdropShaderSampler, NoDeclaration)
{
    // Uses of prefixed samplers, sampler_state blocks and function parameters aren't declarations of a texture.
    EXPECT_TRUE(MilkdropShader::FindUnprefixedSamplers("ret = tex2D(sampler_main, uv).xyz;").empty());
    EXPECT_TRUE(MilkdropShader::FindUnprefixedSamplers("sampler sampler_grad = sampler_state { Texture = <grad>; };").empty());
    EXPECT_TRUE(MilkdropShader::FindUnprefixedSamplers("float3 lookup(sampler s, float2 uv) { return tex2D(s, uv).xyz; }").empty());
    EXPECT_TRUE(MilkdropShader::FindUnprefixedSamplers("float3 mysampler;").empty());
}

TEST(MilkdropShaderSampler, MultipleDeclarations)
{
    EXPECT_EQ(MilkdropShader::FindUnprefixedSamplers("sampler a; sampler b;\nsampler a;"),
              std::set<std::string>({"a", "b"}));
}

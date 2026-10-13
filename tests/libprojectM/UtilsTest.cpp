#include <gtest/gtest.h>

#include <Utils.hpp>

#include <regex>
#include <string>

using libprojectM::Utils::EraseRegexMatches;

TEST(Utils, EraseRegexMatches)
{
    std::string text = "sampler sampler_main;\n"
                       "float3 c = tex2D(sampler_main, uv).xyz;\n"
                       "sampler2D sampler_noise;\n";
    EraseRegexMatches(text, "sampler", std::regex("sampler(2D|3D|)(\\s+|\\().*"));
    EXPECT_EQ(text, "\nfloat3 c = tex2D(sampler_main, uv).xyz;\n\n");
}

TEST(Utils, EraseRegexMatchesFindsMatchCreatedByErase)
{
    // Erasing "sampler2D x;" leaves the first "sampler" followed by a line break, which matches too.
    std::string text = "samplersampler2D x;\n y";
    EraseRegexMatches(text, "sampler", std::regex("sampler(2D|3D|)(\\s+|\\().*"));
    EXPECT_EQ(text, "");
}

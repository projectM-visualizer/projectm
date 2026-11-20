#include <gtest/gtest.h>

#include <HLSLParser.h>
#include <HLSLTree.h>

#include <cstring>
#include <string>

namespace {

std::string Preprocess(const char* source)
{
    M4::Allocator allocator;
    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);

    std::string result;
    if (!parser.ApplyPreprocessor("preprocessor.hlsl", source, std::strlen(source), result))
    {
        return "<failed>";
    }
    return result;
}

} // namespace

TEST(HLSLPreprocessor, MacroValueKeepsWhitespaceBetweenTokens)
{
    auto result = Preprocess("#define decl float value\ndecl = 1.0;\n");

    EXPECT_NE(result.find("float value"), std::string::npos) << result;
}

TEST(HLSLPreprocessor, DeclarationMacroIsNotWrappedInParentheses)
{
    // From "Fed + Geiss - Color Pox Remix".
    auto result = Preprocess("#define smp sampler sampler_manyfish;\nsmp\n");

    EXPECT_NE(result.find("sampler sampler_manyfish;"), std::string::npos) << result;
    EXPECT_EQ(result.find("(sampler"), std::string::npos) << result;
}

TEST(HLSLPreprocessor, ExpressionMacroIsStillWrappedInParentheses)
{
    auto result = Preprocess("#define sum 1 + 2\nfloat x = sum * 3;\n");

    EXPECT_NE(result.find("(1 + 2) * 3"), std::string::npos) << result;
}

TEST(HLSLPreprocessor, MacroWithArgumentsKeepsWhitespace)
{
    auto result = Preprocess("#define DECL(name) float name\nDECL(a) = 1.0;\n");

    EXPECT_NE(result.find("float a"), std::string::npos) << result;
}

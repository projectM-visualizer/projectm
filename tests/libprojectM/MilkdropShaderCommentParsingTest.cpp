#include <gtest/gtest.h>

#include <Utils.hpp>

#include <regex>
#include <set>
#include <string>

using libprojectM::Utils::StripComments;


// =====================================================================
// Test every comment-sensitive code path
// from MilkdropShader.cpp.
// =====================================================================

/**
 * Mirrors MilkdropShader::GetReferencedSamplers() sampler_ extraction.
 * (MilkdropShader.cpp ~lines 512-516)
 */
static auto ExtractSamplerNames(const std::string& program) -> std::set<std::string>
{
    std::set<std::string> samplerNames;
    samplerNames.insert("main");

    std::string const stripped = StripComments(program);

    auto found = stripped.find("sampler_", 0);
    while (found != std::string::npos)
    {
        found += 8;
        size_t const end = stripped.find_first_of(" ;,\n\r)", found);
        if (end != std::string::npos)
        {
            std::string const sampler = stripped.substr(
                static_cast<int>(found), static_cast<int>(end - found));
            if (sampler != "state")
            {
                samplerNames.insert(sampler);
            }
        }
        found = stripped.find("sampler_", found);
    }

    found = stripped.find("texsize_", 0);
    while (found != std::string::npos)
    {
        found += 8;
        size_t const end = stripped.find_first_of(" ;,.\n\r)", found);
        if (end != std::string::npos)
        {
            std::string const sampler = stripped.substr(
                static_cast<int>(found), static_cast<int>(end - found));
            samplerNames.insert(sampler);
        }
        found = stripped.find("texsize_", found);
    }

    return samplerNames;
}


/**
 * Mirrors MilkdropShader::GetReferencedSamplers() GetBlur detection.
 * (MilkdropShader.cpp ~lines 561-576)
 * Returns 0 for none, 1/2/3 for the highest blur level detected.
 */
static auto DetectBlurLevel(const std::string& program) -> int
{
    std::string const stripped = StripComments(program);

    if (stripped.find("GetBlur3") != std::string::npos)
    {
        return 3;
    }
    if (stripped.find("GetBlur2") != std::string::npos)
    {
        return 2;
    }
    if (stripped.find("GetBlur1") != std::string::npos)
    {
        return 1;
    }
    return 0;
}


/**
 * Mirrors PreprocessPresetShader() sampler_state removal.
 * (MilkdropShader.cpp ~lines 342-371)
 * Removes "= sampler_state { ... };" blocks from program, using
 * comment-stripped text for searching.
 */
static void RemoveSamplerStateBlocks(std::string& program)
{
    std::string stripped = StripComments(program);
    auto found = stripped.find("sampler_state");
    while (found != std::string::npos)
    {
        found = stripped.rfind('=', found);
        auto startPos = found;

        found = stripped.find('}', found);
        found = stripped.find(';', found);

        if (found != std::string::npos)
        {
            program.replace(startPos, found - startPos, "");
            stripped = StripComments(program);
        }
        else
        {
            break;
        }

        found = stripped.find("sampler_state");
    }
}


/**
 * Mirrors PreprocessPresetShader() shader_body finding.
 * (MilkdropShader.cpp ~lines 373-376)
 * Returns the position of "shader_body" in the original program,
 * or std::string::npos if not found (ignoring commented instances).
 */
static auto FindShaderBody(const std::string& program) -> size_t
{
    std::string const stripped = StripComments(program);
    return stripped.find("shader_body");
}


/**
 * Mirrors PreprocessPresetShader() brace-counting with comment skipping.
 * (MilkdropShader.cpp ~lines 432-473)
 * Given a string starting just after the opening brace, counts nested
 * braces and returns the position of the matching closing brace,
 * skipping braces inside // and block comments.
 */
static auto CountBracesToClose(const std::string& program, size_t startPos) -> size_t
{
    int bracesOpen = 1;
    size_t pos = startPos;
    for (; pos < program.length() && bracesOpen > 0; ++pos)
    {
        switch (program.at(pos))
        {
            case '/':
                // Skip line comments
                if (pos < program.length() - 1 && program.at(pos + 1) == '/')
                {
                    for (; pos < program.length(); ++pos)
                    {
                        if (program.at(pos) == '\n')
                        {
                            break;
                        }
                    }
                }
                // Skip block comments
                else if (pos < program.length() - 1 && program.at(pos + 1) == '*')
                {
                    pos += 2;
                    for (; pos < program.length() - 1; ++pos)
                    {
                        if (program.at(pos) == '*' && program.at(pos + 1) == '/')
                        {
                            ++pos;
                            break;
                        }
                    }
                }
                continue;

            case '{':
                bracesOpen++;
                continue;

            case '}':
                bracesOpen--;
        }
    }
    return pos;
}


/**
 * Mirrors TranspileHLSLShader() regex-based sampler declaration removal.
 * (MilkdropShader.cpp ~lines 625-629)
 */
static void RegexRemoveSamplerDeclarations(std::string& source)
{
    std::smatch matches;
    while (std::regex_search(source, matches,
                             std::regex("sampler(2D|3D|)(\\s+|\\().*")))
    {
        source.replace(matches.position(), matches.length(), "");
    }
}


/**
 * Mirrors TranspileHLSLShader() regex-based texsize declaration removal.
 * (MilkdropShader.cpp ~lines 632-635)
 */
static void RegexRemoveTexsizeDeclarations(std::string& source)
{
    std::smatch matches;
    while (std::regex_search(source, matches,
                             std::regex("float4\\s+texsize_.*")))
    {
        source.replace(matches.position(), matches.length(), "");
    }
}


// =====================================================================
// StripComments unit tests
// =====================================================================

TEST(StripComments, PreservesCodeWithNoComments)
{
    std::string input = "uniform sampler2D sampler_mytex;\n";
    EXPECT_EQ(StripComments(input), input);
}

TEST(StripComments, StripsLineComment)
{
    std::string result = StripComments("code // comment\nmore code\n");
    EXPECT_EQ(result, "code           \nmore code\n");
}

TEST(StripComments, StripsLineCommentNoSpace)
{
    std::string result = StripComments("//sampler sampler_rand00;\n");
    EXPECT_EQ(result.size(), std::string("//sampler sampler_rand00;\n").size());
    EXPECT_EQ(result.back(), '\n');
    EXPECT_EQ(result.find("sampler"), std::string::npos)
        << "All comment content should be replaced with spaces";
}

TEST(StripComments, StripsBlockComment)
{
    std::string result = StripComments("before /* inside */ after\n");
    EXPECT_EQ(result, "before              after\n");
}

TEST(StripComments, StripsMultiLineBlockComment)
{
    std::string input =
        "before\n"
        "/* line1\n"
        "   line2 */\n"
        "after\n";
    std::string result = StripComments(input);
    EXPECT_NE(result.find("before"), std::string::npos);
    EXPECT_NE(result.find("after"), std::string::npos);
    EXPECT_EQ(result.find("line1"), std::string::npos);
    EXPECT_EQ(result.find("line2"), std::string::npos);
    EXPECT_EQ(result.size(), input.size()) << "String length must be preserved";
}

TEST(StripComments, PreservesNewlinesInBlockComment)
{
    std::string input = "/*\nfoo\nbar\n*/\n";
    std::string result = StripComments(input);
    auto countNewlines = [](const std::string& s) {
        return std::count(s.begin(), s.end(), '\n');
    };
    EXPECT_EQ(countNewlines(result), countNewlines(input));
}

TEST(StripComments, EmptyString)
{
    EXPECT_EQ(StripComments(""), "");
}

TEST(StripComments, UnterminatedBlockComment)
{
    std::string result = StripComments("code /* no close");
    EXPECT_EQ(result.substr(0, 5), "code ");
    EXPECT_EQ(result.find("no"), std::string::npos);
}

TEST(StripComments, PreservesStringLength)
{
    // Every input should produce output of the same length
    std::string inputs[] = {
        "hello // world\n",
        "a /* b */ c\n",
        "// entire line\n",
        "/* multi\nline\ncomment */\n",
        "no comments here\n",
    };
    for (const auto& input : inputs)
    {
        EXPECT_EQ(StripComments(input).size(), input.size())
            << "Length mismatch for: " << input;
    }
}

TEST(StripComments, LineCommentAtEndOfFileNoNewline)
{
    std::string result = StripComments("code // trailing");
    EXPECT_EQ(result, "code            ");
}

TEST(StripComments, NestedSlashesInsideBlockComment)
{
    // "//" inside a block comment should NOT start a line comment
    std::string result = StripComments("/* // not a line comment */ real_code\n");
    EXPECT_NE(result.find("real_code"), std::string::npos)
        << "Code after block comment close must survive";
}

TEST(StripComments, SlashStarInsideLineComment)
{
    // "/*" inside a line comment should NOT start a block comment
    std::string result = StripComments("// /* not block\nreal_code\n");
    EXPECT_NE(result.find("real_code"), std::string::npos)
        << "Code on next line after // /* must survive";
}

TEST(StripComments, ConsecutiveComments)
{
    std::string input = "// first\n// second\ncode;\n";
    std::string result = StripComments(input);
    EXPECT_EQ(result.find("first"), std::string::npos);
    EXPECT_EQ(result.find("second"), std::string::npos);
    EXPECT_NE(result.find("code"), std::string::npos);
}

TEST(StripComments, AdjacentBlockComments)
{
    std::string input = "/* a *//* b */code;\n";
    std::string result = StripComments(input);
    EXPECT_EQ(result.find('a'), std::string::npos);
    EXPECT_EQ(result.find('b'), std::string::npos);
    EXPECT_NE(result.find("code"), std::string::npos);
}

TEST(StripComments, SingleSlashNotComment)
{
    // A lone slash (e.g. division) should not be treated as a comment start
    std::string input = "x = a / b;\n";
    EXPECT_EQ(StripComments(input), input);
}


// =====================================================================
// GetReferencedSamplers: sampler_ / texsize_ extraction
// =====================================================================

TEST(SamplerParsing, FindsUncommentedSampler)
{
    auto names = ExtractSamplerNames("uniform sampler2D sampler_mytex;\n");
    EXPECT_TRUE(names.count("main"));
    EXPECT_TRUE(names.count("mytex"));
}

TEST(SamplerParsing, IgnoresLineCommentedSampler)
{
    auto names = ExtractSamplerNames("// sampler sampler_rand00;\n");
    EXPECT_FALSE(names.count("rand00"))
        << "sampler_rand00 inside a // comment should not be extracted";
}

TEST(SamplerParsing, NoSpaceAfterSlashesStillCommented)
{
    auto names = ExtractSamplerNames("//sampler sampler_rand00;\n");
    EXPECT_FALSE(names.count("rand00"))
        << "sampler_rand00 after //sampler (no space) should not be extracted";
}

TEST(SamplerParsing, IgnoresBlockCommentedSampler)
{
    auto names = ExtractSamplerNames("/* sampler_rand00 */\n");
    EXPECT_FALSE(names.count("rand00"))
        << "sampler_rand00 inside /* */ should not be extracted";
}

TEST(SamplerParsing, BlockCommentSpansMultipleLines)
{
    std::string program =
        "/*\n"
        "  sampler_foo;\n"
        "  texsize_bar;\n"
        "*/\n"
        "uniform sampler2D sampler_real;\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_FALSE(names.count("foo"));
    EXPECT_FALSE(names.count("bar"));
    EXPECT_TRUE(names.count("real"));
}

TEST(SamplerParsing, SamplerAfterBlockCommentCloseIsFound)
{
    auto names = ExtractSamplerNames("/* comment */ sampler_visible;\n");
    EXPECT_TRUE(names.count("visible"));
}

TEST(SamplerParsing, IgnoresLineCommentedTexsize)
{
    auto names = ExtractSamplerNames("// texsize_rand00;\n");
    EXPECT_FALSE(names.count("rand00"));
}

TEST(SamplerParsing, IgnoresBlockCommentedTexsize)
{
    auto names = ExtractSamplerNames("/* texsize_rand00; */\n");
    EXPECT_FALSE(names.count("rand00"));
}

TEST(SamplerParsing, TexsizeInBlockCommentSamplerOutside)
{
    std::string program =
        "/* texsize_hidden; */\n"
        "float4 ts = texsize_visible;\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_FALSE(names.count("hidden"));
    EXPECT_TRUE(names.count("visible"));
}

TEST(SamplerParsing, MixedCommentedAndUncommented)
{
    std::string program =
        "//sampler sampler_rand00;\n"
        "uniform sampler2D sampler_tex1;\n"
        "/* sampler_hidden; */\n"
        "uniform sampler2D sampler_tex2;\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_FALSE(names.count("rand00"));
    EXPECT_FALSE(names.count("hidden"));
    EXPECT_TRUE(names.count("tex1"));
    EXPECT_TRUE(names.count("tex2"));
}

TEST(SamplerParsing, SkipsSamplerState)
{
    auto names = ExtractSamplerNames("sampler_state { Filter = LINEAR; };\n");
    EXPECT_FALSE(names.count("state"))
        << "sampler_state is a reserved word and should be skipped";
}

TEST(SamplerParsing, CommentedSamplerStateNotExtracted)
{
    auto names = ExtractSamplerNames("// sampler_state { Filter = LINEAR; };\n");
    EXPECT_EQ(names.size(), 1u)
        << "Only 'main' should be present; entire line is a comment";
}

TEST(SamplerParsing, EmptyProgram)
{
    auto names = ExtractSamplerNames("");
    EXPECT_EQ(names.size(), 1u);
    EXPECT_TRUE(names.count("main"));
}

TEST(SamplerParsing, ReproducesCrashPresetPattern)
{
    std::string program =
        "//sampler sampler_rand00;    // this will choose a random texture from disk!\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_EQ(names.size(), 1u);
    EXPECT_FALSE(names.count("rand00"));
}

TEST(SamplerParsing, MultipleSamplersOnSameLinePartiallyCommented)
{
    // sampler_a is real code, sampler_b is in a trailing comment
    std::string program = "sampler_a; // sampler_b;\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_TRUE(names.count("a"));
    EXPECT_FALSE(names.count("b"))
        << "sampler_b after // on the same line should be ignored";
}

TEST(SamplerParsing, SamplerInsideBlockCommentWithCodeOnSameLine)
{
    std::string program = "real /* sampler_fake; */ sampler_real;\n";
    auto names = ExtractSamplerNames(program);
    EXPECT_FALSE(names.count("fake"));
    EXPECT_TRUE(names.count("real"));
}


// =====================================================================
// GetReferencedSamplers: GetBlur detection
// =====================================================================

TEST(BlurDetection, NoBlur)
{
    EXPECT_EQ(DetectBlurLevel("ret = tex2D(sampler_main, uv);\n"), 0);
}

TEST(BlurDetection, DetectsBlur1)
{
    EXPECT_EQ(DetectBlurLevel("ret = GetBlur1(uv);\n"), 1);
}

TEST(BlurDetection, DetectsBlur2)
{
    EXPECT_EQ(DetectBlurLevel("ret = GetBlur2(uv);\n"), 2);
}

TEST(BlurDetection, DetectsBlur3)
{
    EXPECT_EQ(DetectBlurLevel("ret = GetBlur3(uv);\n"), 3);
}

TEST(BlurDetection, Blur3TakesPrecedenceOverBlur1)
{
    // GetBlur3 should be detected even if GetBlur1 also appears
    EXPECT_EQ(DetectBlurLevel("a = GetBlur1(uv);\nb = GetBlur3(uv);\n"), 3);
}

TEST(BlurDetection, IgnoresBlur1InLineComment)
{
    EXPECT_EQ(DetectBlurLevel("// ret = GetBlur1(uv);\n"), 0)
        << "GetBlur1 inside // comment should not be detected";
}

TEST(BlurDetection, IgnoresBlur2InLineComment)
{
    EXPECT_EQ(DetectBlurLevel("// ret = GetBlur2(uv);\n"), 0)
        << "GetBlur2 inside // comment should not be detected";
}

TEST(BlurDetection, IgnoresBlur3InLineComment)
{
    EXPECT_EQ(DetectBlurLevel("// ret = GetBlur3(uv);\n"), 0)
        << "GetBlur3 inside // comment should not be detected";
}

TEST(BlurDetection, IgnoresBlurInBlockComment)
{
    EXPECT_EQ(DetectBlurLevel("/* GetBlur3(uv) */\n"), 0)
        << "GetBlur3 inside /* */ comment should not be detected";
}

TEST(BlurDetection, CommentedBlur3RealBlur1)
{
    std::string program =
        "// GetBlur3(uv);\n"
        "ret = GetBlur1(uv);\n";
    EXPECT_EQ(DetectBlurLevel(program), 1)
        << "Commented GetBlur3 should be ignored; only real GetBlur1 counts";
}

TEST(BlurDetection, BlockCommentedBlur2RealBlur1)
{
    std::string program =
        "/* GetBlur2(uv); */\n"
        "ret = GetBlur1(uv);\n";
    EXPECT_EQ(DetectBlurLevel(program), 1)
        << "Block-commented GetBlur2 should be ignored; only real GetBlur1 counts";
}

TEST(BlurDetection, AllBlurLevelsCommentedOut)
{
    std::string program =
        "// GetBlur1(uv);\n"
        "/* GetBlur2(uv); */\n"
        "// GetBlur3(uv);\n";
    EXPECT_EQ(DetectBlurLevel(program), 0)
        << "All blur calls are commented; blur level should be 0";
}


// =====================================================================
// PreprocessPresetShader: sampler_state removal
// =====================================================================

TEST(SamplerStateRemoval, RemovesRealSamplerState)
{
    std::string program =
        "sampler mytex = sampler_state { Filter = LINEAR; };\n"
        "shader_body { }\n";
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program.find("sampler_state"), std::string::npos)
        << "Real sampler_state block should be removed";
    EXPECT_NE(program.find("shader_body"), std::string::npos)
        << "shader_body should be preserved";
}

TEST(SamplerStateRemoval, SkipsLineCommentedSamplerState)
{
    std::string program =
        "float x = 1.0;\n"
        "// sampler mytex = sampler_state { Filter = LINEAR; };\n"
        "shader_body { }\n";
    std::string original = program;
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program, original)
        << "Commented sampler_state should not trigger any removal";
}

TEST(SamplerStateRemoval, SkipsBlockCommentedSamplerState)
{
    std::string program =
        "float x = 1.0;\n"
        "/* sampler mytex = sampler_state { Filter = LINEAR; }; */\n"
        "shader_body { }\n";
    std::string original = program;
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program, original)
        << "Block-commented sampler_state should not trigger any removal";
}

TEST(SamplerStateRemoval, RemovesRealButSkipsCommented)
{
    std::string program =
        "// sampler a = sampler_state { Filter = POINT; };\n"
        "sampler b = sampler_state { Filter = LINEAR; };\n"
        "shader_body { }\n";
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program.find("Filter = LINEAR"), std::string::npos)
        << "The real sampler_state block should be removed";
    EXPECT_NE(program.find("Filter = POINT"), std::string::npos)
        << "The commented sampler_state block should be untouched";
}

TEST(SamplerStateRemoval, MultipleRealSamplerStates)
{
    std::string program =
        "sampler a = sampler_state { Filter = POINT; };\n"
        "sampler b = sampler_state { Filter = LINEAR; };\n"
        "shader_body { }\n";
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program.find("sampler_state"), std::string::npos)
        << "Both sampler_state blocks should be removed";
}

TEST(SamplerStateRemoval, NoSamplerState)
{
    std::string program = "shader_body { }\n";
    std::string original = program;
    RemoveSamplerStateBlocks(program);
    EXPECT_EQ(program, original);
}


// =====================================================================
// PreprocessPresetShader: shader_body finding
// =====================================================================

TEST(ShaderBodyFind, FindsRealShaderBody)
{
    std::string program = "shader_body\n{\n}\n";
    size_t pos = FindShaderBody(program);
    EXPECT_EQ(pos, 0u);
}

TEST(ShaderBodyFind, SkipsLineCommentedShaderBody)
{
    std::string program = "// shader_body\nshader_body\n{\n}\n";
    size_t pos = FindShaderBody(program);
    EXPECT_NE(pos, std::string::npos);
    // Should find the one on the second line, not the first
    EXPECT_GT(pos, program.find('\n'))
        << "Should skip the commented shader_body and find the real one";
}

TEST(ShaderBodyFind, SkipsBlockCommentedShaderBody)
{
    std::string program = "/* shader_body */ shader_body\n{\n}\n";
    size_t pos = FindShaderBody(program);
    EXPECT_NE(pos, std::string::npos);
    // The real one starts after the block comment
    EXPECT_GT(pos, static_cast<size_t>(0));
}

TEST(ShaderBodyFind, AllShaderBodiesCommented)
{
    std::string program = "// shader_body\n/* shader_body */\n{\n}\n";
    size_t pos = FindShaderBody(program);
    EXPECT_EQ(pos, std::string::npos)
        << "No real shader_body exists; should return npos";
}

TEST(ShaderBodyFind, ShaderBodyAfterCodeAndComments)
{
    std::string program =
        "// some commented shader_body\n"
        "float4 x = 1.0;\n"
        "/* another shader_body */\n"
        "shader_body\n{\n}\n";
    size_t pos = FindShaderBody(program);
    EXPECT_NE(pos, std::string::npos);
    // Should be on the last "shader_body" line
    std::string before = program.substr(0, pos);
    // Count newlines before the found position — should be 3
    auto newlines = std::count(before.begin(), before.end(), '\n');
    EXPECT_EQ(newlines, 3)
        << "Real shader_body should be on the 4th line (after 3 newlines)";
}


// =====================================================================
// PreprocessPresetShader: brace counting with comment skipping
// =====================================================================

TEST(BraceCounting, SimpleNesting)
{
    // "{ a { b } c }" — start after first {, should stop after last }
    std::string program = " a { b } c }";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}');
    // After processing, bracesOpen should be 0 at position of last '}'
    EXPECT_EQ(pos, program.length());
}

TEST(BraceCounting, SkipsBracesInLineComment)
{
    // Braces inside // comment should not count
    std::string program = " // { } \n}";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}')
        << "Should find the real closing brace after the comment";
}

TEST(BraceCounting, SkipsBracesInBlockComment)
{
    // Braces inside /* */ should not count
    std::string program = " /* { } */ }";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}')
        << "Should skip braces inside /* */ and find the real closing brace";
}

TEST(BraceCounting, NestedBracesWithBlockComment)
{
    // Real nesting plus commented braces that should be ignored
    std::string program = " { /* } */ } }";
    size_t pos = CountBracesToClose(program, 0);
    // bracesOpen starts at 1. Sees '{' (+1=2), skips '}' in comment,
    // sees '}' (-1=1), sees '}' (-1=0) => stops
    EXPECT_EQ(program.at(pos - 1), '}');
}

TEST(BraceCounting, MultipleLineComments)
{
    std::string program =
        " // }\n"
        " // }\n"
        " }\n";
    size_t pos = CountBracesToClose(program, 0);
    // Only the real } on the third line should count
    EXPECT_EQ(program.at(pos - 1), '}');
}

TEST(BraceCounting, MixedCommentTypes)
{
    std::string program =
        " // { \n"
        " /* } */ \n"
        " { \n"
        " } \n"
        " }\n";
    size_t pos = CountBracesToClose(program, 0);
    // bracesOpen: 1, skip //{, skip /*}*/, see { (+1=2), see } (-1=1), see } (-1=0)
    EXPECT_EQ(program.at(pos - 1), '}');
}

TEST(BraceCounting, BlockCommentSpanningMultipleLines)
{
    std::string program =
        " /*\n"
        " {\n"
        " }\n"
        " */\n"
        " }\n";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}')
        << "Braces inside multi-line block comment should be ignored";
}

TEST(BraceCounting, SlashThatIsNotComment)
{
    // Division operator should not confuse the brace counter
    std::string program = " float x = a / b; }";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}');
}

TEST(BraceCounting, LineCommentImmediatelyFollowedByBrace)
{
    // Edge case: closing brace on line right after comment with no space
    std::string program = "// comment\n}";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}');
}

TEST(BraceCounting, BlockCommentContainingSlashSlash)
{
    // "//" inside block comment should NOT start a line comment
    std::string program = " /* // } */ }";
    size_t pos = CountBracesToClose(program, 0);
    EXPECT_EQ(program.at(pos - 1), '}')
        << "// inside /* */ should not start a line comment; "
           "the } after */ should close the brace";
}


// =====================================================================
// TranspileHLSLShader: regex-based sampler declaration removal
// =====================================================================

TEST(RegexSamplerRemoval, RemovesUncommentedDeclaration)
{
    std::string source = "uniform sampler2D sampler_tex;\n";
    RegexRemoveSamplerDeclarations(source);
    EXPECT_EQ(source.find("sampler2D"), std::string::npos)
        << "sampler2D declaration should be removed";
}

TEST(RegexSamplerRemoval, Removes3DDeclaration)
{
    std::string source = "uniform sampler3D sampler_vol;\n";
    RegexRemoveSamplerDeclarations(source);
    EXPECT_EQ(source.find("sampler3D"), std::string::npos);
}

TEST(RegexSamplerRemoval, RemovesBareSamplerDeclaration)
{
    // "sampler" without 2D/3D suffix
    std::string source = "sampler mytex;\n";
    RegexRemoveSamplerDeclarations(source);
    EXPECT_EQ(source.find("sampler"), std::string::npos);
}

TEST(RegexSamplerRemoval, MatchesCommentedDeclaration)
{
    // The regex operates on HLSL preprocessor output which preserves
    // comments. This test documents that commented declarations WILL
    // be matched by the regex (known low-risk limitation).
    std::string source = "// sampler2D sampler_tex;\nfloat x = 1.0;\n";
    RegexRemoveSamplerDeclarations(source);
    // The regex matches "sampler2D sampler_tex;" even inside the comment
    // because the regex has no comment awareness. It will eat from
    // "sampler2D" to end of line including everything after.
    EXPECT_EQ(source.find("sampler2D"), std::string::npos)
        << "Regex matches commented declarations too (known limitation)";
}

TEST(RegexSamplerRemoval, PreservesUnrelatedCode)
{
    std::string source = "float4 x = 1.0;\nsampler2D mytex;\nfloat4 y = 2.0;\n";
    RegexRemoveSamplerDeclarations(source);
    EXPECT_NE(source.find("float4 x"), std::string::npos)
        << "Unrelated code before the declaration should survive";
    EXPECT_NE(source.find("float4 y"), std::string::npos)
        << "Unrelated code after the declaration should survive";
}

TEST(RegexSamplerRemoval, RemovesMultipleDeclarations)
{
    std::string source =
        "sampler2D tex1;\n"
        "float x = 1.0;\n"
        "sampler3D tex2;\n";
    RegexRemoveSamplerDeclarations(source);
    EXPECT_EQ(source.find("sampler2D"), std::string::npos);
    EXPECT_EQ(source.find("sampler3D"), std::string::npos);
    EXPECT_NE(source.find("float x"), std::string::npos);
}


// =====================================================================
// TranspileHLSLShader: regex-based texsize declaration removal
// =====================================================================

TEST(RegexTexsizeRemoval, RemovesUncommentedDeclaration)
{
    std::string source = "float4 texsize_main;\n";
    RegexRemoveTexsizeDeclarations(source);
    EXPECT_EQ(source.find("texsize_main"), std::string::npos);
}

TEST(RegexTexsizeRemoval, MatchesCommentedDeclaration)
{
    // Documents that commented texsize declarations are also removed
    // (known low-risk limitation — regex has no comment awareness).
    std::string source = "// float4 texsize_main;\nfloat x = 1.0;\n";
    RegexRemoveTexsizeDeclarations(source);
    EXPECT_EQ(source.find("texsize_main"), std::string::npos)
        << "Regex matches commented declarations too (known limitation)";
}

TEST(RegexTexsizeRemoval, PreservesUnrelatedCode)
{
    std::string source = "float4 x = 1.0;\nfloat4 texsize_rand00;\nfloat4 y = 2.0;\n";
    RegexRemoveTexsizeDeclarations(source);
    EXPECT_NE(source.find("float4 x"), std::string::npos);
    EXPECT_NE(source.find("float4 y"), std::string::npos);
}

TEST(RegexTexsizeRemoval, DoesNotMatchFloat3)
{
    // "float3 texsize_..." should NOT match — only float4 does
    std::string source = "float3 texsize_main;\n";
    std::string original = source;
    RegexRemoveTexsizeDeclarations(source);
    EXPECT_EQ(source, original)
        << "float3 texsize_ should not be matched by the float4 regex";
}

TEST(RegexTexsizeRemoval, RemovesMultipleDeclarations)
{
    std::string source =
        "float4 texsize_rand00;\n"
        "float x = 1.0;\n"
        "float4 texsize_rand01;\n";
    RegexRemoveTexsizeDeclarations(source);
    EXPECT_EQ(source.find("texsize_rand00"), std::string::npos);
    EXPECT_EQ(source.find("texsize_rand01"), std::string::npos);
    EXPECT_NE(source.find("float x"), std::string::npos);
}

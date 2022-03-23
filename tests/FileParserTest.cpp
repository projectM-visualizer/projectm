#include <gtest/gtest.h>

#include <MilkdropPresetFactory/FileParser.hpp>

static constexpr auto fileParserTestDataPath{ PROJECTM_TEST_DATA_DIR "/FileParser/" };

/**
 * Class to make protected function accessible to tests.
 */
class FileParserMock : public FileParser
{
public:
    static void StripComment(std::string& line)
    {
        FileParser::StripComment(line);
    }

    static void StripMultilineComment(std::string& code)
    {
        FileParser::StripMultilineComment(code);
    }

    static void Trim(std::string& line)
    {
        FileParser::Trim(line);
    }
};

TEST(FileParser, ReadEmptyFile)
{
    FileParser parser;
    ASSERT_FALSE(parser.Read(std::string(fileParserTestDataPath) + "parser-empty.milk"));
}

TEST(FileParser, ReadFileWithNullByte)
{
    FileParser parser;
    ASSERT_FALSE(parser.Read(std::string(fileParserTestDataPath) + "parser-nullbyte.milk"));
}

TEST(FileParser, ReadSimpleFile)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));
}

TEST(FileParser, GetRawPresetValues)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    EXPECT_FALSE(values.empty());
}

TEST(FileParser, EmptyValue)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    // Lines with empty values should be ignored
    EXPECT_TRUE(values.find("empty_value") == values.end());
}

TEST(FileParser, EmptyKey)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    // Lines with empty key should be ignored
    ASSERT_TRUE(values.find("value_with_space") != values.end());
    EXPECT_EQ(values.at("value_with_space"), "123");
}

TEST(FileParser, ValueWithSpaceDelimiter)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    // Lines with empty key should be ignored
    EXPECT_TRUE(values.find("empty_key") == values.end());
}

TEST(FileParser, ReadFileWithRepeatedKey)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-repeatedkey.milk"));

    const auto& values = parser.PresetValues();

    ASSERT_TRUE(values.find("warp") != values.end());
    EXPECT_EQ(values.at("warp"), "0");
}

TEST(FileParser, GetCode)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_");
    EXPECT_EQ(code, "r=1.0; g=1.0; b=1.0; ");
}

TEST(FileParser, GetCodeWithGap)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_gap_");
    EXPECT_EQ(code, "r=1.0; g=1.0; ");
}

TEST(FileParser, GetCodeWithRepeatedLine)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_repeat_");
    EXPECT_EQ(code, "r=1.0; g=1.0; b=1.0; ");
}

TEST(FileParser, GetCodeTrimmed)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_trim_");
    EXPECT_EQ(code, "r = 1.0; g = 1.0; b = 1.0; ");
}

TEST(FileParser, GetCodeMultilineComment)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("multiline_comment_");
    EXPECT_EQ(code, "r = 1.0; b = 1.0; ");
}

TEST(FileParser, GetCodeShaderSyntax)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("warp_");
    EXPECT_EQ(code, "r=1.0;\ng=1.0;\nb=1.0;\n");
}

TEST(FileParser, GetIntValid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("nVideoEchoOrientation", 0), 3);
}

TEST(FileParser, GetIntInvalid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("nSomeWeirdStuff", 123), 123);
}

TEST(FileParser, GetIntDefault)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("RandomKey", 123), 123);
}

TEST(FileParser, GetFloatValid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("fVideoEchoAlpha", 0), 0.5f);
}

TEST(FileParser, GetFloatInvalid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("fSomeWeirdStuff", 123.0f), 123.0f);
}

TEST(FileParser, GetFloatDefault)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("RandomKey", 123.0f), 123.0f);
}

TEST(FileParser, GetBooleanValid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("bAdditiveWaves", false), true);
}

TEST(FileParser, GetBooleanInvalid)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("bSomeWeirdStuff", true), true);
}

TEST(FileParser, GetBooleanDefault)
{
    FileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("RandomKey", true), true);
}

TEST(FileParser, StripCommentBegin)
{
    std::string line{ "// Full line comment" };
    FileParserMock::StripComment(line);
    ASSERT_EQ(line, "");
}

TEST(FileParser, StripCommentLeadingWhitespace)
{
    std::string line{ "    // Full line comment" };
    FileParserMock::StripComment(line);
    ASSERT_EQ(line, "    ");
}

TEST(FileParser, StripCommentLeadingText)
{
    std::string line{ "1.005// Some value" };
    FileParserMock::StripComment(line);
    ASSERT_EQ(line, "1.005");
}

TEST(FileParser, StripCommentEmptyComment)
{
    std::string line{ "1.005//" };
    FileParserMock::StripComment(line);
    ASSERT_EQ(line, "1.005");
}

TEST(FileParser, StripMultiLineCommentOnlyComment)
{
    std::string code{ "/* Full line comment */" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "");
}

TEST(FileParser, StripMultiLineCommentMiddleComment)
{
    std::string code{ "Some /* Middle line comment */ Text" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "Some  Text");
}

TEST(FileParser, StripMultiLineCommentMultipleComments)
{
    std::string code{ "Some /* Middle */ More /* line */ Nice /* comment */ Text" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "Some  More  Nice  Text");
}

TEST(FileParser, StripMultiLineCommentWithLinebreak)
{
    // Mot really a usecase as newlines are stripped, but should work.
    std::string code{ "/* Multi\nline\ncomment */" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "");
}

TEST(FileParser, StripMultiLineCommentWithWrongTerminatorBeginning)
{
    std::string code{ "*/ Text /* Comment with termination in front" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "*/ Text ");
}

TEST(FileParser, StripMultiLineCommentWithWrongTerminatorEnd)
{
    std::string code{ "Text /* Comment with two terminators */ */" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "Text  */");
}

TEST(FileParser, StripMultiLineCommentWithoutTerminator)
{
    std::string code{ "Text /* Comment without termination" };
    FileParserMock::StripMultilineComment(code);
    ASSERT_EQ(code, "Text ");
}

TEST(FileParser, TrimFront)
{
    std::string line{ "    TEXT TEXT" };
    FileParserMock::Trim(line);
    ASSERT_EQ(line, "TEXT TEXT");
}

TEST(FileParser, TrimBack)
{
    std::string line{ "TEXT TEXT    " };
    FileParserMock::Trim(line);
    ASSERT_EQ(line, "TEXT TEXT");
}

TEST(FileParser, TrimBoth)
{
    std::string line{ "    TEXT TEXT    " };
    FileParserMock::Trim(line);
    ASSERT_EQ(line, "TEXT TEXT");
}

TEST(FileParser, TrimOtherWhitespace)
{
    std::string line{ "\t \v TEXT TEXT \r \n \f" };
    FileParserMock::Trim(line);
    ASSERT_EQ(line, "TEXT TEXT");
}

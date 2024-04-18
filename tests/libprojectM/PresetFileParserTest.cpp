#include <gtest/gtest.h>

#include <MilkdropPreset/PresetFileParser.hpp>

static constexpr auto fileParserTestDataPath{ PROJECTM_TEST_DATA_DIR "/PresetFileParser/" };

using libprojectM::MilkdropPreset::PresetFileParser;

/**
 * Class to make protected function accessible to tests.
 */
class PresetFileParserMock : public PresetFileParser
{
public:
};

TEST(PresetFileParser, ReadEmptyFile)
{
    PresetFileParser parser;
    ASSERT_FALSE(parser.Read(std::string(fileParserTestDataPath) + "parser-empty.milk"));
}

TEST(PresetFileParser, ReadFileWithNullByte)
{
    PresetFileParser parser;
    ASSERT_FALSE(parser.Read(std::string(fileParserTestDataPath) + "parser-nullbyte.milk"));
}

TEST(PresetFileParser, ReadSimpleFile)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));
}

TEST(PresetFileParser, GetRawPresetValues)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    EXPECT_FALSE(values.empty());
}

TEST(PresetFileParser, EmptyValue)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    // Lines with empty values should be stored as such
    ASSERT_FALSE(values.find("empty_value") == values.end());
    EXPECT_EQ(values.at("empty_value"), "");
}

TEST(PresetFileParser, ValueWithSpaceDelimiter)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    ASSERT_TRUE(values.find("value_with_space") != values.end());
    EXPECT_EQ(values.at("value_with_space"), "123");
}

TEST(PresetFileParser, EmptyKey)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-simple.milk"));

    const auto& values = parser.PresetValues();

    // Lines with empty key should be ignored
    EXPECT_TRUE(values.find("empty_key") == values.end());
}

TEST(PresetFileParser, ReadFileWithRepeatedKey)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-repeatedkey.milk"));

    const auto& values = parser.PresetValues();

    ASSERT_TRUE(values.find("warp") != values.end());
    EXPECT_EQ(values.at("warp"), "0");
}

TEST(PresetFileParser, GetCode)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_");
    EXPECT_EQ(code, "r=1.0;\ng=1.0;\n\nb=1.0;\n");
}

TEST(PresetFileParser, GetCodeWithGap)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_gap_");
    EXPECT_EQ(code, "r=1.0;\ng=1.0;\n");
}

TEST(PresetFileParser, GetCodeWithRepeatedLine)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("per_frame_repeat_");
    EXPECT_EQ(code, "r=1.0;\ng=1.0;\nb=1.0;\n");
}

TEST(PresetFileParser, GetCodeMultilineComment)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("multiline_comment_");
    EXPECT_EQ(code, "r = 1.0; /* Comment...\ng = 1.0;\n... ends here */b = 1.0;\n");
}

TEST(PresetFileParser, GetCodeShaderSyntax)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-code.milk"));

    auto code = parser.GetCode("warp_");
    EXPECT_EQ(code, "r=1.0;\ng=1.0;\nb=1.0;\n");
}

TEST(PresetFileParser, GetIntValid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("nVideoEchoOrientation", 0), 3);
}

TEST(PresetFileParser, GetIntInvalid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("nSomeWeirdStuff", 123), 123);
}

TEST(PresetFileParser, GetIntDefault)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetInt("RandomKey", 123), 123);
}

TEST(PresetFileParser, GetFloatValid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("fVideoEchoAlpha", 0), 0.5f);
}

TEST(PresetFileParser, GetFloatInvalid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("fSomeWeirdStuff", 123.0f), 123.0f);
}

TEST(PresetFileParser, GetFloatDefault)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_FLOAT_EQ(parser.GetFloat("RandomKey", 123.0f), 123.0f);
}

TEST(PresetFileParser, GetBooleanValid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("bAdditiveWaves", false), true);
}

TEST(PresetFileParser, GetBooleanInvalid)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("bSomeWeirdStuff", true), true);
}

TEST(PresetFileParser, GetBooleanDefault)
{
    PresetFileParser parser;
    ASSERT_TRUE(parser.Read(std::string(fileParserTestDataPath) + "parser-valueconversion.milk"));

    EXPECT_EQ(parser.GetBool("RandomKey", true), true);
}

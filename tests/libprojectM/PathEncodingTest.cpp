#include <gtest/gtest.h>

#include <MilkdropPreset/PresetFileParser.hpp>
#include <PathEncoding.hpp>

#include <cstdio>
#include <string>

using libprojectM::PathEncoding::FromNarrowApi;
using libprojectM::PathEncoding::OpenFile;
using libprojectM::PathEncoding::ToNarrowApi;

namespace {

// "B_<U+26A0>_Warning.milk" in UTF-8. U+26A0 isn't part of any ANSI code page.
const std::string WarningFileUtf8{"B_\xE2\x9A\xA0_Warning.milk"};

} // namespace


TEST(PathEncoding, AsciiIsUnchanged)
{
    EXPECT_EQ(FromNarrowApi("C:/presets/A preset.milk"), "C:/presets/A preset.milk");
    EXPECT_EQ(ToNarrowApi("C:/presets/A preset.milk"), "C:/presets/A preset.milk");
    EXPECT_EQ(FromNarrowApi(nullptr), "");
    EXPECT_EQ(FromNarrowApi(""), "");
}


#ifdef _WIN32

TEST(PathEncoding, Utf8AndUtf16RoundTrip)
{
    using libprojectM::PathEncoding::Utf8ToWide;
    using libprojectM::PathEncoding::WideToUtf8;

    EXPECT_EQ(Utf8ToWide(WarningFileUtf8), L"B_\u26A0_Warning.milk");
    EXPECT_EQ(WideToUtf8(L"B_\u26A0_Warning.milk"), WarningFileUtf8);
    EXPECT_EQ(WideToUtf8(static_cast<const wchar_t*>(nullptr)), "");
}


TEST(PathEncoding, NarrowApiUsesTheAnsiCodePage)
{
    if (GetACP() != 1252)
    {
        GTEST_SKIP() << "This test needs the Windows-1252 code page.";
    }

    // "caf<U+00E9>": E9 in Windows-1252, C3 A9 in UTF-8.
    EXPECT_EQ(FromNarrowApi("caf\xE9"), "caf\xC3\xA9");
    EXPECT_EQ(ToNarrowApi("caf\xC3\xA9"), "caf\xE9");
    // Not representable in Windows-1252.
    EXPECT_EQ(ToNarrowApi(WarningFileUtf8), "B_?_Warning.milk");
}


// A preset file whose name the ANSI code page can't represent can be read by its UTF-8 name.
TEST(PathEncoding, ReadPresetWithUnicodeFileName)
{
    wchar_t tempDirectory[MAX_PATH + 1]{};
    ASSERT_GT(GetTempPathW(MAX_PATH, tempDirectory), 0u);
    const std::wstring widePath = std::wstring(tempDirectory) + L"projectM_test_B_\u26A0_Warning.milk";
    const std::string utf8Path = libprojectM::PathEncoding::WideToUtf8(widePath);

    {
        FILE* file = OpenFile(utf8Path, "wb");
        ASSERT_NE(file, nullptr);
        const std::string contents = "[preset00]\nzoom=1.25\n";
        std::fwrite(contents.data(), 1, contents.size(), file);
        std::fclose(file);
    }

    libprojectM::MilkdropPreset::PresetFileParser parser;
    const bool read = parser.Read(utf8Path);
    _wremove(widePath.c_str());

    ASSERT_TRUE(read);
    EXPECT_FLOAT_EQ(parser.GetFloat("zoom", 0.0f), 1.25f);
}

#endif

#include "PresetFileParser.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

auto PresetFileParser::Read(const std::string& presetFile) -> bool
{
    std::ifstream presetStream(presetFile.c_str(), std::ios_base::in | std::ios_base::binary);
    return Read(presetStream);
}

auto PresetFileParser::Read(std::istream& presetStream) -> bool
{
    if (!presetStream.good())
    {
        return false;
    }

    presetStream.seekg(0, presetStream.end);
    auto fileSize = presetStream.tellg();
    presetStream.seekg(0, presetStream.beg);

    if (static_cast<size_t>(fileSize) > maxFileSize)
    {
        return false;
    }

    std::vector<char> presetFileContents(fileSize);
    presetStream.read(presetFileContents.data(), fileSize);

    if (presetStream.fail() || presetStream.bad())
    {
        return false;
    }

    size_t startPos{0}; //!< Starting position of current line
    size_t pos{0};      //!< Current read position

    auto parseLineIfDataAvailable = [this, &pos, &startPos, &presetFileContents]() {
        if (pos > startPos)
        {
            auto beg = presetFileContents.begin();
            std::string line(beg + startPos, beg + pos);
            ParseLine(line);
        }
    };

    while (pos < presetFileContents.size())
    {
        switch (presetFileContents[pos])
        {
            case '\r':
            case '\n':
                // EOL, skip over CRLF
                parseLineIfDataAvailable();
                startPos = pos + 1;
                break;

            case '\0':
                // Null char is not expected. Could be a random binary file.
                return false;
        }

        ++pos;
    }

    parseLineIfDataAvailable();

    return !m_presetValues.empty();
}

auto PresetFileParser::GetCode(const std::string& keyPrefix) const -> std::string
{
    auto lowerKey = ToLower(keyPrefix);

    std::stringstream code;                        //!< The parsed code
    std::string key(lowerKey.length() + 5, '\0'); //!< Allocate a string that can hold up to 5 digits.

    key.replace(0, lowerKey.length(), lowerKey);

    for (int index{1}; index <= 99999; ++index)
    {
        key.replace(lowerKey.length(), 5, std::to_string(index));
        if (m_presetValues.find(key) == m_presetValues.end())
        {
            break;
        }

        auto line = m_presetValues.at(key);

        // Remove backtick char in shader code
        if (!line.empty() && line.at(0) == '`')
        {
            line.erase(0, 1);
        }
        code << line << std::endl;
    }

    auto codeStr = code.str();

    return codeStr;
}

namespace {

/**
 * @brief Returns whether two characters at the end and start of two code lines can't be separated.
 *
 * Milkdrop joins the lines without a separator. Where that can merge the two characters into
 * one token, e.g. parts of a name or number, or operators like "+=" or "//", the lines must be
 * joined. Everywhere else, a line break gives the same tokens and keeps the line numbers.
 */
auto MustJoin(char last, char first) -> bool
{
    auto canMerge = [](char character) {
        switch (character)
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '\v':
            case '\f':
            case ';':
            case ',':
            case '(':
            case ')':
            case '[':
            case ']':
            case '?':
            case ':':
                return false;
            default:
                return true;
        }
    };

    return canMerge(last) && canMerge(first);
}

} // anonymous namespace

auto PresetFileParser::GetEquationCode(const std::string& keyPrefix) const -> EquationCode
{
    auto lowerKey = ToLower(keyPrefix);

    EquationCode code;
    std::string key(lowerKey.length() + 5, '\0'); //!< Allocate a string that can hold up to 5 digits.

    key.replace(0, lowerKey.length(), lowerKey);

    char lastCharacter{'\n'}; //!< Last character of the code so far.
    int pendingLineBreaks{};   //!< Line breaks between the code so far and the next line with code.

    for (int index{1}; index <= 99999; ++index)
    {
        key.replace(lowerKey.length(), 5, std::to_string(index));
        if (m_presetValues.find(key) == m_presetValues.end())
        {
            break;
        }

        const auto& line = m_presetValues.at(key);

        // Same as Milkdrop's ReadCode() and StripLinefeedCharsAndComments(): Skip a leading backtick and cut off
        // // and \\ comments until the end of the line.
        size_t start = (!line.empty() && line.at(0) == '`') ? 1 : 0;
        size_t end = start;
        while (end < line.length())
        {
            if (end + 1 < line.length() &&
                ((line[end] == '/' && line[end + 1] == '/') || (line[end] == '\\' && line[end + 1] == '\\')))
            {
                break;
            }
            ++end;
        }

        if (index > 1)
        {
            pendingLineBreaks++;
        }

        if (end == start)
        {
            continue;
        }

        // Milkdrop joins the lines without a separator. Keep the line breaks where this gives the same tokens, so
        // compiler errors keep the line numbers of the preset.
        if (code.Empty() || !MustJoin(lastCharacter, line[start]))
        {
            for (; pendingLineBreaks > 0; pendingLineBreaks--)
            {
                code.AppendLineBreak();
            }
        }
        pendingLineBreaks = 0;

        code.Append(line.substr(start, end - start), index, static_cast<int>(start) + 1);
        lastCharacter = line[end - 1];
    }

    return code;
}

auto PresetFileParser::GetInt(const std::string& key, int defaultValue) -> int
{
    auto lowerKey = ToLower(key);
    if (m_presetValues.find(lowerKey) != m_presetValues.end())
    {
        try
        {
            return std::stoi(m_presetValues.at(lowerKey));
        }
        catch (std::logic_error&)
        {
        }
    }

    return defaultValue;
}

auto PresetFileParser::GetFloat(const std::string& key, float defaultValue) -> float
{
    auto lowerKey = ToLower(key);
    if (m_presetValues.find(lowerKey) != m_presetValues.end())
    {
        try
        {
            return std::stof(m_presetValues.at(lowerKey));
        }
        catch (std::logic_error&)
        {
        }
    }

    return defaultValue;
}

auto PresetFileParser::GetBool(const std::string& key, bool defaultValue) -> bool
{
    return GetInt(key, static_cast<int>(defaultValue)) > 0;
}

auto PresetFileParser::GetString(const std::string& key, const std::string& defaultValue) -> std::string
{
    auto lowerKey = ToLower(key);
    if (m_presetValues.find(lowerKey) != m_presetValues.end())
    {
        return m_presetValues.at(lowerKey);
    }

    return defaultValue;
}

const std::map<std::string, std::string>& PresetFileParser::PresetValues() const
{
    return m_presetValues;
}

void PresetFileParser::ParseLine(const std::string& line)
{
    // Search for first delimiter, either space or equal
    auto varNameDelimiterPos = line.find_first_of(" =");

    if (varNameDelimiterPos == std::string::npos || varNameDelimiterPos == 0)
    {
        // Empty line, delimiter at start of line or no delimiter found, skip.
        return;
    }

    // Convert key to lower case, as INI functions are not case-sensitive.
    std::string varName(ToLower(std::string(line.begin(), line.begin() + varNameDelimiterPos)));
    std::string value(line.begin() + varNameDelimiterPos + 1, line.end());

    // Only add first occurrence to mimic Milkdrop behaviour
    if (!varName.empty() && m_presetValues.find(varName) == m_presetValues.end())
    {
        m_presetValues.emplace(std::move(varName), std::move(value));
    }
}

auto PresetFileParser::ToLower(std::string str) -> std::string
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); }
    );

    return str;
}

} // namespace MilkdropPreset
} // namespace libprojectM

#include "FileParser.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

bool FileParser::Read(const std::string& presetFile)
{
    std::ifstream presetStream(presetFile.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!presetStream.good())
    {
        return false;
    }

    presetStream.seekg(0, presetStream.end);
    auto fileSize = presetStream.tellg();
    presetStream.seekg(0, presetStream.beg);

    if (fileSize > maxFileSize)
    {
        return false;
    }

    std::vector<char> presetFileContents(fileSize);
    presetStream.read(presetFileContents.data(), fileSize);

    if (presetStream.fail() || presetStream.bad())
    {
        return false;
    }

    presetStream.close();

    size_t startPos{ 0 }; //!< Starting position of current line
    size_t pos{ 0 }; //!< Current read position

    auto parseLineIfDataAvailable = [this, &pos, &startPos, &presetFileContents]()
    {
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

std::string FileParser::GetCode(const std::string& keyPrefix) const
{
    std::stringstream code; //!< The parsed code
    std::string key(keyPrefix.length() + 5, '\0'); //!< Allocate a string that can hold up to 5 digits.

    key.replace(0, keyPrefix.length(), keyPrefix);

    for (int index{ 1 }; index <= 99999; ++index)
    {
        key.replace(keyPrefix.length(), 5, std::to_string(index));
        if (m_presetValues.find(key) == m_presetValues.end())
        {
            break;
        }

        auto line = m_presetValues.at(key);
        StripComment(line);
        Trim(line);

        if (!line.empty())
        {
            if (line.at(0) == '`')
            {
                // Append newline in shader code
                line.erase(0, 1);
                code << line << std::endl;
            }
            else
            {
                // Append a space in equation code
                code << line << " ";
            }
        }
    }

    auto codeStr = code.str();
    StripMultilineComment(codeStr);

    return codeStr;
}

int FileParser::GetInt(const std::string& key, int defaultValue)
{
    if (m_presetValues.find(key) != m_presetValues.end())
    {
        try
        {
            return std::stoi(m_presetValues.at(key));
        }
        catch (std::logic_error& ex)
        {
        }
    }

    return defaultValue;
}

float FileParser::GetFloat(const std::string& key, float defaultValue)
{
    if (m_presetValues.find(key) != m_presetValues.end())
    {
        try
        {
            return std::stof(m_presetValues.at(key));
        }
        catch (std::logic_error& ex)
        {
        }
    }

    return defaultValue;
}

int FileParser::GetBool(const std::string& key, bool defaultValue)
{
    return GetInt(key, static_cast<int>(defaultValue)) > 0;
}

const std::map<std::string, std::string>& FileParser::PresetValues() const
{
    return m_presetValues;
}

void FileParser::ParseLine(const std::string& line)
{
    // Search for first delimiter, either space or equal
    auto varNameDelimiterPos = line.find_first_of(" =");

    if (varNameDelimiterPos >= line.length() || varNameDelimiterPos == 0)
    {
        // Empty line, delimiter at end of line or no delimiter found, skip.
        return;
    }

    std::string varName(line.begin(), line.begin() + varNameDelimiterPos);
    std::string value(line.begin() + varNameDelimiterPos + 1, line.end());

    // Only add first occurrence to mimic Milkdrop behaviour
    if (!varName.empty() && !value.empty() && m_presetValues.find(varName) == m_presetValues.end())
    {
        m_presetValues.emplace(std::move(varName), std::move(value));
    }
}

void FileParser::StripComment(std::string& line)
{
    auto commentPos = line.find("//");
    if (commentPos != std::string::npos)
    {
        line.resize(commentPos);
    }

    // While not documented, Milkdrop also considers "\\" to be a comment.
    commentPos = line.find("\\\\");
    if (commentPos != std::string::npos)
    {
        line.resize(commentPos);
    }
}

void FileParser::StripMultilineComment(std::string& code)
{
    size_t commentPos;
    while((commentPos = code.find("/*")) != std::string::npos)
    {
        auto endPos = code.find("*/");
        if (endPos != std::string::npos && endPos > commentPos)
        {
            code.erase(commentPos, endPos - commentPos + 2);
        }
        else
        {
            code.erase(commentPos, code.size() - commentPos);
        }
    }
}

void FileParser::Trim(std::string& line)
{
    if (line.empty())
    {
        return;
    }

    line.erase(line.begin(), std::find_if(line.begin(), line.end(),
                                          std::not1(std::ptr_fun<int, int>(std::isspace))));
    line.erase(std::find_if(line.rbegin(), line.rend(),
                            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), line.end());
}

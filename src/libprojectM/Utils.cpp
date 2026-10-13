#include "Utils.hpp"

#include <algorithm>

namespace libprojectM {
namespace Utils {

auto ToLower(const std::string& str) -> std::string
{
    std::string lowerStr(str);
    ToLowerInPlace(lowerStr);
    return lowerStr;
}

auto ToUpper(const std::string& str) -> std::string
{
    std::string upperStr(str);
    ToUpperInPlace(upperStr);
    return upperStr;
}

void ToLowerInPlace(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void ToUpperInPlace(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

auto StripComments(const std::string& source) -> std::string
{
    std::string result = source;
    size_t i = 0;

    while (i < result.size())
    {
        if (i + 1 < result.size() && result.at(i) == '/' && result.at(i + 1) == '/')
        {
            // Line comment: replace until end of line
            while (i < result.size() && result.at(i) != '\n' && result.at(i) != '\r')
            {
                result.at(i) = ' ';
                i++;
            }
        }
        else if (i + 1 < result.size() && result.at(i) == '/' && result.at(i + 1) == '*')
        {
            // Block comment: replace until closing */
            result.at(i) = ' ';
            result.at(i + 1) = ' ';
            i += 2;
            while (i < result.size())
            {
                if (i + 1 < result.size() && result.at(i) == '*' && result.at(i + 1) == '/')
                {
                    result.at(i) = ' ';
                    result.at(i + 1) = ' ';
                    i += 2;
                    break;
                }
                // Preserve newlines to keep line structure intact
                if (result.at(i) != '\n' && result.at(i) != '\r')
                {
                    result.at(i) = ' ';
                }
                i++;
            }
        }
        else
        {
            i++;
        }
    }

    return result;
}

void EraseRegexMatches(std::string& text, const std::string& prefix, const std::regex& pattern)
{
    std::smatch match;
    size_t pos = 0;
    while ((pos = text.find(prefix, pos)) != std::string::npos)
    {
        if (std::regex_search(text.cbegin() + pos, text.cend(), match, pattern, std::regex_constants::match_continuous))
        {
            text.erase(pos, match.length());
            pos = 0; // The erase can create a new match further up.
        }
        else
        {
            pos++;
        }
    }
}

} // namespace Utils
} // namespace libprojectM

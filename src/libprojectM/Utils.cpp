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

} // namespace Utils
} // namespace libprojectM

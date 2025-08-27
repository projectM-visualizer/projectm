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

}
}

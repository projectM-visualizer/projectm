#pragma once

#include <regex>
#include <string>

namespace libprojectM {
namespace Utils {

auto ToLower(const std::string& str) -> std::string;
auto ToUpper(const std::string& str) -> std::string;

void ToLowerInPlace(std::string& str);
void ToUpperInPlace(std::string& str);

/**
 * @brief Strips C and C++ style comments from source code.
 *
 * Replaces // line comments and block comments with spaces, preserving
 * string length and newline positions so that character offsets remain valid.
 *
 * @param source The source code string to strip comments from.
 * @return A copy of the source with all comment content replaced by spaces.
 */
auto StripComments(const std::string& source) -> std::string;

/**
 * @brief Erases all matches of a regex from a string.
 *
 * Only tries the regex where prefix occurs, which is much faster than std::regex_search().
 *
 * @param text The string to erase the matches from.
 * @param prefix The literal text every match starts with.
 * @param pattern The regex to erase.
 */
void EraseRegexMatches(std::string& text, const std::string& prefix, const std::regex& pattern);

} // namespace Utils
} // namespace libprojectM

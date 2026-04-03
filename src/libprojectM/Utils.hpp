#pragma once

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

} // namespace Utils
} // namespace libprojectM

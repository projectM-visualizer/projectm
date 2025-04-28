#pragma once

#include <string>

namespace libprojectM {
namespace Utils {

auto ToLower(const std::string& str) -> std::string;
auto ToUpper(const std::string& str) -> std::string;

void ToLowerInPlace(std::string& str);
void ToUpperInPlace(std::string& str);

} // namespace Utils
} // namespace libprojectM

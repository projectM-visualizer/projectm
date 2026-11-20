#include "EquationCode.hpp"

#include <algorithm>

namespace libprojectM {
namespace MilkdropPreset {

EquationCode::EquationCode(const std::string& code)
{
    m_code.reserve(code.size());

    size_t start{};
    while (start < code.size())
    {
        auto end = code.find('\n', start);
        if (end == std::string::npos)
        {
            end = code.size();
        }

        // Comments end at the end of their line: once joined, they would hide the following lines.
        auto line = code.substr(start, end - start);
        auto const comment = std::min(line.find("//"), line.find("\\\\"));
        if (comment != std::string::npos)
        {
            line.erase(comment);
        }

        m_lineStarts.push_back(m_code.size());
        m_code += line;
        start = end + 1;
    }
}

auto EquationCode::Code() const -> const std::string&
{
    return m_code;
}

auto EquationCode::OriginalPosition(int offset) const -> Position
{
    if (m_lineStarts.empty() || offset < 0)
    {
        return {1, 1};
    }

    // Last line starting at or before the offset. Empty lines start where the next line starts,
    // so a character is always given to the line it was read from.
    auto const position = std::min(static_cast<size_t>(offset), m_code.size());
    auto const line = std::upper_bound(m_lineStarts.begin(), m_lineStarts.end(), position) - 1;

    return {static_cast<int>(line - m_lineStarts.begin()) + 1,
            static_cast<int>(position - *line) + 1};
}

} // namespace MilkdropPreset
} // namespace libprojectM
